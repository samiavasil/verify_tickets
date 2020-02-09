#ifndef AJRSERVER_H
#define AJRSERVER_H

#include "TCPServer.h"

class AJRServer : public TCPServer
{
public:
    AJRServer(QObject *parent = nullptr,   const ServerConfigurator &config = ServerConfigurator());
protected:
    virtual void Receive();
    void ParseJsonInput(const QByteArray &buff);
};

#endif // AJRSERVER_H
