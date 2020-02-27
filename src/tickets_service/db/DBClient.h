#ifndef DBCLIENT_H
#define DBCLIENT_H
#include<QObject>
#include<QString>
#include <cassandra.h>

typedef struct Credentials_ {
  const char* password;
  const char* username;
} Credentials;


class DBClient:public QObject
{
    Q_OBJECT
public:
    enum connectionState{
        NOT_CONNECTED,
        CONNECTED
    };

    static DBClient& Instance();
    virtual ~DBClient();
    bool connectSession();

    QString hosts() const;
    void setHosts(const QString &hosts);

    Credentials credentials() const;
    void setCredentials(const Credentials &credentials);

    int protocol() const;
    void setProtocol(int protocol);

    connectionState connState() const;

    CassSession *session() const;
protected:
    DBClient();

protected:
    Credentials m_credentials;
    QString m_hosts;
    int m_protocol;
    CassCluster* m_cluster;
    CassSession* m_session;
    connectionState m_connState;
};

#endif // DBCLIENT_H
