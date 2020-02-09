#include "QRServer.h"

QRServer::QRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}

QRServer::~QRServer()
{

}

void QRServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << clientSocket->readAll();
    clientSocket->write("Server says Hello");
}
