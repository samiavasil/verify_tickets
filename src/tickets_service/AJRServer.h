#ifndef AJRSERVER_H
#define AJRSERVER_H

#include "TCPServer.h"

class AJRServer : public TCPServer
{
public:

    AJRServer(QObject *parent = nullptr,   const ServerConfigurator &config = ServerConfigurator());
protected:
    enum DataStatus {
        NO_ERROR,
        ERROR
    };
    virtual void Receive();
    DataStatus ParseJsonInput(const QByteArray &buff);
};

#endif // AJRSERVER_H
