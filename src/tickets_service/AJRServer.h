#ifndef AJRSERVER_H
#define AJRSERVER_H

#include "TCPServer.h"
#include "db/AJRSale.h"

class AJRServer : public TCPServer
{
public:

    AJRServer(QObject *parent = nullptr,   const ServerConfigurator &config = ServerConfigurator());
protected:
    virtual void Receive();
    inline bool ParseJsonInput(QByteArray &buff, QList<QMap<AJRSale::Column_t, QVariant> > &out);
    bool TransferSoldAccess(QList<QMap<AJRSale::Column_t, QVariant> > &data);
    bool ProcessAjurData(QList<QMap<AJRSale::Column_t , QVariant>> &data);

protected:
    int m_lastId;
private:
    void prepareTables();
};

#endif // AJRSERVER_H
