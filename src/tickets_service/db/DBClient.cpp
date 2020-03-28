#include "DBClient.h"
#include <QDebug>

void on_auth_initial(CassAuthenticator* auth, void* data) {
    /*
   * This callback is used to initiate a request to begin an authentication
   * exchange. Required resources can be acquired and initialized here.
   *
   * Resources required for this specific exchange can be stored in the
   * auth->data field and will be available in the subsequent challenge
   * and success phases of the exchange. The cleanup callback should be used to
   * free these resources.
   */

    /*
   * The data parameter contains the credentials passed in when the
   * authentication callbacks were set and is available to all
   * authentication exchanges.
   */
    const DBClient *client = static_cast<const DBClient*>(data);
    Q_ASSERT(client != nullptr);
    const Credentials credentials = client->credentials();

    size_t username_size = strlen(credentials.username);
    size_t password_size = strlen(credentials.password);
    size_t size = username_size + password_size + 2;

    char* response = cass_authenticator_response(auth, size);

    /* Credentials are prefixed with '\0' */
    response[0] = '\0';
    memcpy(response + 1, credentials.username, username_size);

    response[username_size + 1] = '\0';
    memcpy(response + username_size + 2, credentials.password, password_size);
}

void on_auth_challenge(CassAuthenticator* auth, void* data, const char* token, size_t token_size) {
    /*
   * Not used for plain text authentication, but this is to be used
   * for handling an authentication challenge initiated by the server.
   */
    qDebug() << __FUNCTION__ << ": Authentication chalenge.";
}

void on_auth_success(CassAuthenticator* auth, void* data, const char* token, size_t token_size) {
    /*
   * Not used for plain text authentication, but this is to be used
   * for handling the success phase of an exchange.
   */
    qDebug() << __FUNCTION__ << ": Authentication success!!";
}

void on_auth_cleanup(CassAuthenticator* auth, void* data) {
    /*
   * No resources cleanup is necessary for plain text authentication, but
   * this is used to cleanup resources acquired during the authentication
   * exchange.
   */
    qDebug() << __FUNCTION__ << ": Authentication cleanup!!";
}

void on_host_listener(CassHostListenerEvent event, CassInet inet, void* data) {
    /* Get the string representation of the inet address */
    char address[CASS_INET_STRING_LENGTH];
    cass_inet_string(inet, address);
    const DBClient *client = static_cast<const DBClient*>(data);
    Q_ASSERT(client != nullptr);

    /* Perform application logic for host listener event */
    if (event == CASS_HOST_LISTENER_EVENT_ADD) {
        qDebug() << "Host " << address << " has been ADDED";
    } else if (event == CASS_HOST_LISTENER_EVENT_REMOVE) {
        qDebug() << "Host " << address << " has been REMOVED";
    } else if (event == CASS_HOST_LISTENER_EVENT_UP) {
        qDebug() << "Host " << address << " is UP";
    } else if (event == CASS_HOST_LISTENER_EVENT_DOWN) {
        qDebug() << "Host " << address << " is DOWN";
    }
}

//DBClient::DBClient():m_credentials({ "museum", "cassandra" }),
//
DBClient::DBClient():m_credentials({ "museum", "cassandra" }),
    m_hosts("192.168.4.221,192.168.4.222, 192.168.2.221,192.168.2.222"),
    m_protocol(4),
    m_cluster(nullptr),
    m_session(nullptr),
    m_connState(NOT_CONNECTED)
{

}

DBClient &DBClient::Instance()
{
    static DBClient m_client;
    return m_client;
}

DBClient::~DBClient(){

    if(m_cluster)
        cass_cluster_free(m_cluster);
    if(m_session)
        cass_session_free(m_session);
}

bool DBClient::connectSession()
{
    bool ret = false;
    CassFuture* connect_future = nullptr;
    CassAuthenticatorCallbacks auth_callbacks = {
        on_auth_initial, on_auth_challenge,
        on_auth_success, on_auth_cleanup
    };

    m_cluster = cass_cluster_new();
    m_session = cass_session_new();

    cass_cluster_set_host_listener_callback(m_cluster, on_host_listener, this);

    cass_cluster_set_protocol_version(m_cluster, m_protocol);

    cass_cluster_set_contact_points(m_cluster, m_hosts.toLocal8Bit().constData());

    /* Set custom authentication callbacks and credentials */
    cass_cluster_set_authenticator_callbacks(m_cluster, &auth_callbacks, nullptr, this);

    /* Provide the cluster object as configuration to connect the session */
    connect_future = cass_session_connect(m_session, m_cluster);

    if (cass_future_error_code(connect_future) == CASS_OK) {
        qDebug() << "Successfully connected!";
        ret = true;
    } else {
        /* Handle error */
        const char* message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        qDebug() << "Unable to connect: " << message_length << message;
    }

    cass_future_free(connect_future);

    if(ret)
        m_connState = CONNECTED;

    return ret;
}

QString DBClient::hosts() const
{
    return m_hosts;
}

void DBClient::setHosts(const QString &hosts)
{
    m_hosts = hosts;
}

Credentials DBClient::credentials() const
{
    return m_credentials;
}

void DBClient::setCredentials(const Credentials &credentials)
{
    m_credentials = credentials;
}

int DBClient::protocol() const
{
    return m_protocol;
}

void DBClient::setProtocol(int protocol)
{
    m_protocol = protocol;
}

DBClient::connectionState DBClient::connState() const
{
    return m_connState;
}

CassSession *DBClient::session() const
{
    return m_session;
}
