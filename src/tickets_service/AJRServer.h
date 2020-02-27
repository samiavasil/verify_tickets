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
    inline bool ParseJsonInput(QByteArray &buff, QList<Ajr_data_t> &out);
    bool ProcessAjurData(QList<Ajr_data_t> &data);
protected:

    int m_lastId;
    bool InserRowInSalesTable(Ajr_data_t &data);
};

#endif // AJRSERVER_H
