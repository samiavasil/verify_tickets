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
    inline bool ParseJsonInput(QByteArray &buff, QList<QMap<QString, QVariant> > &out);
    bool ProcessAjurData(QList<QMap<QString, QVariant> > &data);
protected:
    int m_lastId;

};

#endif // AJRSERVER_H
