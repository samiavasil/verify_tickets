#include "TCPServer.h"
#include <QDebug>
#include <QFile>

TCPServer::TCPServer(QObject *parent, const ServerConfigurator& config)
    :QTcpServer(parent), m_config(config)
{

    if(m_config.is_encrypted()) {
        qDebug()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
        qDebug()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
        QFile keyFile(m_config.getKey());
        keyFile.open(QIODevice::ReadOnly);
        m_key = QSslKey(keyFile.readAll(), QSsl::Rsa);
        keyFile.close();

        QFile certFile(m_config.getCert());
        certFile.open(QIODevice::ReadOnly);
        m_cert = QSslCertificate(certFile.readAll());
        certFile.close();
    }

    if (!listen(m_config.getIP(), m_config.getPort())) {
        qCritical() << "Unable to start the TCP server";
        exit(0);
    }
    connect(this, &TCPServer::newConnection, this, &TCPServer::linkConnection);

}

TCPServer::~TCPServer()
{

}

void TCPServer::incomingConnection(qintptr socketDescriptor)
{
    if(m_config.is_encrypted()) {
        createSslSocket(socketDescriptor);
    } else {
        createRegularSocket(socketDescriptor);
    }

}

QTcpSocket* TCPServer::createSslSocket(qintptr socketDescriptor) {

    QSslSocket *serverSocket = new QSslSocket;

    if (serverSocket->setSocketDescriptor(socketDescriptor)) {

        serverSocket->setPrivateKey(m_key);
        serverSocket->setLocalCertificate(m_cert);
        serverSocket->addCaCertificates(m_config.getCa());
        serverSocket->setPeerVerifyMode(QSslSocket::VerifyPeer);
        connect(serverSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslErrors(QList<QSslError>)));
        connect(serverSocket, &QSslSocket::encrypted, this, &TCPServer::EncryptionReady);
        serverSocket->startServerEncryption();
        addPendingConnection(serverSocket);

    } else {
        delete serverSocket;
        serverSocket = nullptr;
    }
    return serverSocket;
}

QTcpSocket* TCPServer::createRegularSocket(qintptr socketDescriptor) {

    QTcpSocket *serverSocket = new QTcpSocket;
    if (serverSocket->setSocketDescriptor(socketDescriptor)) {
        addPendingConnection(serverSocket);
    } else {
        delete serverSocket;
        serverSocket = nullptr;
    }
    return serverSocket;
}

void TCPServer::EncryptionReady()
{

}

void TCPServer::sslErrors(const QList<QSslError> &errors)
{
    foreach (const QSslError &error, errors)
        qDebug() << error.errorString();
}

void TCPServer::linkConnection()
{
    QTcpSocket *clientSocket;

    clientSocket = nextPendingConnection();
    connect(clientSocket, &QTcpSocket::readyRead, this, &TCPServer::Receive);
    connect(clientSocket, &QTcpSocket::disconnected, this, &TCPServer::Disconnected);
}


void TCPServer::Disconnected()
{
    qDebug("Client Disconnected");
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    clientSocket->deleteLater();
}

