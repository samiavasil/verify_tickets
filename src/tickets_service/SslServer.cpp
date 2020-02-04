#include "SslServer.h"
#include <QDebug>
#include <QFile>


SslServer::SslServer(QObject *parent, const QHostAddress &address, quint16 port, bool bencr)
    :QTcpServer(parent)
{

    QFile keyFile("../../certificates/red_local.key");
    keyFile.open(QIODevice::ReadOnly);
    m_key = QSslKey(keyFile.readAll(), QSsl::Rsa);
    keyFile.close();

    QFile certFile("../../certificates/red_local.pem");
    certFile.open(QIODevice::ReadOnly);
    m_cert = QSslCertificate(certFile.readAll());
    certFile.close();

    if (!listen(address, port)) {
        qCritical() << "Unable to start the TCP server";
        exit(0);
    }
    connect(this, &SslServer::newConnection, this, &SslServer::linkConnection);

}

SslServer::~SslServer()
{

}

void SslServer::incomingConnection(qintptr socketDescriptor)
{
    QSslSocket *serverSocket = new QSslSocket;
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {

        serverSocket->setPrivateKey(m_key);
        serverSocket->setLocalCertificate(m_cert);
        serverSocket->addCaCertificates("../../certificates/blue_ca.pem");
        serverSocket->setPeerVerifyMode(QSslSocket::VerifyPeer);
        connect(serverSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
        connect(serverSocket, &QSslSocket::encrypted, this, &SslServer::EncriptionReady);
        serverSocket->startServerEncryption();
        addPendingConnection(serverSocket);

    } else {
        delete serverSocket;
    }
}

void SslServer::EncriptionReady()
{

}

void SslServer::sslErrors(const QList<QSslError> &errors)
{
    foreach (const QSslError &error, errors)
        qDebug() << error.errorString();
}

void SslServer::linkConnection()
{
    QTcpSocket *clientSocket;

    clientSocket = nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &SslServer::Receive);
    connect(clientSocket, &QTcpSocket::disconnected, this, &SslServer::Disconnected);
}

void SslServer::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    qDebug() << clientSocket->readAll();
    clientSocket->write("Server says Hello");
}

void SslServer::Disconnected()
{
    qDebug("Client Disconnected");
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    clientSocket->deleteLater();
}

