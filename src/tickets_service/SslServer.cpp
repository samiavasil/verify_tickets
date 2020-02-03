#include<QSslSocket>
#include "SslServer.h"


SslServer::SslServer(QObject *parent)
{

}

SslServer::~SslServer()
{

}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *serverSocket = new QSslSocket;
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(serverSocket);
        connect(serverSocket, &QSslSocket::encrypted, this, &SslServer::ready);
        serverSocket->startServerEncryption();
    } else {
        delete serverSocket;
    }
}

void SslServer::ready()
{


}
