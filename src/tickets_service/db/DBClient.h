#ifndef DBCLIENT_H
#define DBCLIENT_H
#include<QObject>
#include<QString>
#include <cassandra.h>

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
    connectionState connState() const;
    CassSession *session() const;
protected:
    DBClient();

protected:
    CassCluster* m_cluster;
    CassSession* m_session;
    connectionState m_connState;
};

#endif // DBCLIENT_H
