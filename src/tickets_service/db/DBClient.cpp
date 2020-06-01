#include "DBClient.h"
#include <QDebug>
#include "Configurator.h"

/**
  TODO: Create connection.reconection staregy !!!!
*/

#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qDebug() << "Error:" << (text) << __func__ << " Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    } \
    while (0)

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
    QString user = Configurator::Instance().user();
    QString password = Configurator::Instance().password();

    size_t username_size = strlen(user.toUtf8().constData());
    size_t password_size = strlen(password.toUtf8().constData());
    size_t size = username_size + password_size + 2;

    char* response = cass_authenticator_response(auth, size);

    /* Credentials are prefixed with '\0' */
    response[0] = '\0';
    memcpy(response + 1, user.toUtf8().constData(), username_size);

    response[username_size + 1] = '\0';
    memcpy(response + username_size + 2, password.toUtf8().constData(), password_size);
}

void on_auth_challenge(CassAuthenticator* auth, void* data, const char* token, size_t token_size) {
    /*
   * Not used for plain text authentication, but this is to be used
   * for handling an authentication challenge initiated by the server.
   */
    (void)auth;
    (void)data;
    (void)token;
    (void)token_size;
    qDebug() << __FUNCTION__ << ": Authentication chalenge.";
}

void on_auth_success(CassAuthenticator* auth, void* data, const char* token, size_t token_size) {
    /*
   * Not used for plain text authentication, but this is to be used
   * for handling the success phase of an exchange.
   */
    (void)auth;
    (void)data;
    (void)token;
    (void)token_size;
    qDebug() << __FUNCTION__ << ": Authentication success!!";
}

void on_auth_cleanup(CassAuthenticator* auth, void* data) {
    /*
   * No resources cleanup is necessary for plain text authentication, but
   * this is used to cleanup resources acquired during the authentication
   * exchange.
   */
    (void)auth;
    (void)data;
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

DBClient::DBClient():
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

    cass_cluster_set_protocol_version(m_cluster, Configurator::Instance().protocol());

    cass_cluster_set_contact_points(m_cluster, Configurator::Instance().hosts().
                                    toLocal8Bit().constData());

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

    if(ret) {
        PrepareTables();
        m_connState = CONNECTED;
    }

    return ret;
}

DBClient::connectionState DBClient::connState() const
{
    return m_connState;
}

CassSession *DBClient::session() const
{
    return m_session;
}

void DBClient::DoConsistencyTransfer() {
    QList<QMap<QString, QVariant>> sales;

    //TBD: Here is good idea to have a querry paging
    m_AJRSale.SelectFromTable(sales, "*", "transfered=false ALLOW FILTERING");
    qDebug() << "DO Consistency check";
    foreach (auto sale, sales) {
        QList<QMap<QString, QVariant>> data ({{
                                                             {"aj_site_id", sale.value("aj_site_id", -1)},
                                                             {"sale_id", sale.value("sale_id", -1)},
                                                             {"code", sale.value("code", -1)},
                                                             {"transfered", sale.value("transfered", -1)}
                                                         }});

        DBClient::Instance().TransferSoldAccess(data);
    }
}

void DBClient::PrepareTables() {
    //TODO: REMove me

    //   m_AJRSale.setKeySpace("Vesko_Space");

    /* TBD: Should be deprecated - Creating keyspaces and network topology should be
       defined externaly not in aspplication. App will know only the name of keyspace.*/
    /* qDebug() << "Create KeySpace: " << m_AJRSale.CreateKeySpace();*/

    //   qDebug() << "Drop Table m_AJRSale: " << m_AJRSale.DropTable();
    qDebug() << "Create Table ajrSale: " << m_AJRSale.CreateTable();

    QList<QMap<QString, QVariant>> result;
    qDebug() << "Dump Table " << m_AJRSale.SelectFromTable(result);

    /*qDebug() << "Drop Table codeAccessInfo: " << m_CodeAccessInfo.DropTable();*/
    qDebug() << "Create Table code_Access_info: " << m_CodeAccessInfo.CreateTable();
    /*qDebug() << "Prepare Table code_Access_info: " << m_CodeAccessInfo.PrepareCodeAccessTable();*/

    qDebug() << "Dump Table CodeAccessInfo" << m_CodeAccessInfo.SelectFromTable(result);

    /* qDebug() << "Drop Table deadTickets: " <<m_DeadTickets.DropTable();*/
    qDebug() << "Create Table deadTickets: " <<m_DeadTickets.CreateTable();

    /* qDebug() << "Drop Table Doors: " << m_Doors.DropTable();*/
    qDebug() << "Create Table Doors: " << m_Doors.CreateTable();

    /* qDebug() << "Drop Table fiscUnit: " << m_FiscUnit.DropTable(); */
    qDebug() << "Create Table fiscUnit: " << m_FiscUnit.CreateTable();
    /* qDebug() << "PrepareFiscUnitTable: " << m_FiscUnit.PrepareFiscUnitTable();*/

    /* qDebug() << "Drop Table siteDescriptor: " << m_SiteDescriptor.DropTable();*/
    qDebug() << "Create Table SiteDescriptor: " << m_SiteDescriptor.CreateTable();
    /*qDebug() << "PrepareSiteDescriptorTable: " << m_SiteDescriptor.PrepareSiteDescriptorTable();*/

    /* qDebug() << "Drop Table soldAccess: " << m_SoldAccess.DropTable(); */
    qDebug() << "Create Table soldAccess: " << m_SoldAccess.CreateTable();
}

bool DBClient::TransferSoldAccess(QList<QMap<QString, QVariant> > &data) {
    int i;
    bool status = false;
    bool is_ok;
    QList<QMap<QString, QVariant>> code_access;

    for (i = 0; i < data.count(); i++) {
        QList<QMap<QString , QVariant>> soldData({{
                {"aj_site_id", data[i].value("aj_site_id", -1)},
                {"sale_id", data[i].value("sale_id", -1)},
                }});

        ASSERT_ERROR("SelectFromTable: ",m_CodeAccessInfo.
                     SelectFromTable(code_access, "*", QString("code = '%1'").
                                     arg(data[i].value("code").toString())));

        ASSERT_ERROR("Code acces isn't defined correctly in code_access table", code_access.count() == 1);

        int access_type = code_access[0].value("access_type").toInt(&is_ok);
        ASSERT_ERROR("Get ACCESS_TYPE", is_ok);
        int dead_level = code_access[0].value("deadlevel").toInt(&is_ok);
        ASSERT_ERROR("Get DEAD_LEVEL", is_ok);

        qDebug() << "ACCESS_TYPE  :"  << access_type;

        if (access_type != CodeAccessInfo::SINGLE) {

            ASSERT_ERROR("Get SIDE_ID", is_ok);
            if (dead_level > 0) {
                QList<QMap<QString , QVariant>> deadTicket({{
                          {"aj_site_id", data[0].value("aj_site_id", -1)},
                          {"sale_id", data[0].value("sale_id", -1)},
                          {"live_ctr", dead_level},
                          }});
                ASSERT_ERROR("SoldAccess Insert row: ",
                            m_DeadTickets.InsertRowsInTable(deadTicket));
            }

            foreach (auto site_id, code_access[0].value("site_ids").toList()) {
                soldData[0]["qr_site_id"] = site_id.toInt(&is_ok);
                ASSERT_ERROR("Get SIDE_ID", is_ok);
                ASSERT_ERROR("SoldAccess Insert row: ",
                             m_SoldAccess.InsertRowsInTable(soldData));
            }

        }

        data[i]["transfered"] = true;
        QList<QMap<QString, QVariant> > transfered_data = {data[i]};
        ASSERT_ERROR("AJRSale Insert row: ", InsertRowsInAjrSale(transfered_data));
    }
    if(i > 0) {
        status = true;
    }
RET_ERROR:
    return status;
}

bool DBClient::GetFiscalMem(QList<QMap<QString, QVariant>>& fisc_unit,
                            const QString &filter, const QString &where ) {
     return m_FiscUnit.SelectFromTable(fisc_unit, filter, where);
}

bool DBClient::GetAjrSales(QList<QMap<QString, QVariant>> &ajr_sale,
                           const QString &filter, const QString &where ) {
    return m_AJRSale.SelectFromTable(ajr_sale, filter, where);
}

bool DBClient::GetCodeAccessInfo(QList<QMap<QString, QVariant>> &code_access,
                                 const QString &filter, const QString &where ) {
    return m_CodeAccessInfo.SelectFromTable(code_access, filter, where);

}

bool DBClient::GetSoldAccess(QList<QMap<QString, QVariant>> &sold_access,
                                 const QString &filter, const QString &where ) {
    return m_SoldAccess.SelectFromTable(sold_access, filter, where);
}

bool DBClient::GetDeadTickets(QList<QMap<QString, QVariant>> &dead_tickets,
                                 const QString &filter, const QString &where ) {
    return m_DeadTickets.SelectFromTable(dead_tickets, filter, where);
}

bool DBClient::InserRowsInDeadTickets(const QList<QMap<QString, QVariant> > &dead_tickets_rows) {
    return m_DeadTickets.InsertRowsInTable(dead_tickets_rows);
}

bool DBClient::InsertRowsInSoldAccessTable(const QList<QMap<QString , QVariant>> &sold_data_rows) {
    return m_SoldAccess.InsertRowsInTable(sold_data_rows);
}

bool DBClient::InsertRowsInAjrSale(const QList<QMap<QString , QVariant>> &sold_data_rows) {
    return m_AJRSale.InsertRowsInTable(sold_data_rows);
}
