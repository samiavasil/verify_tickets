#ifndef QRSERVER_H
#define QRSERVER_H

#include "TCPServer.h"



class QRServer : public TCPServer
{

public:
    QRServer(QObject *parent = nullptr,   const ServerConfigurator &config = ServerConfigurator());
    virtual ~QRServer();
protected:
    virtual void Receive();
};

#endif // QRSERVER_H
