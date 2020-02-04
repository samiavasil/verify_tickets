#include "TCPServer.h"
#include <QDebug>
#include <QFile>
#include <QJsonDocument>


TCPServer::TCPServer(QObject *parent, const QHostAddress &address, quint16 port, bool encrypt)
    :QTcpServer(parent), m_encrypt(encrypt)
{

    if(encrypt) {
        qDebug()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
        qDebug()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
        QFile keyFile("../../certificates/red_local.key");
        keyFile.open(QIODevice::ReadOnly);
        m_key = QSslKey(keyFile.readAll(), QSsl::Rsa);
        keyFile.close();

        QFile certFile("../../certificates/red_local.pem");
        certFile.open(QIODevice::ReadOnly);
        m_cert = QSslCertificate(certFile.readAll());
        certFile.close();
    }

    if (!listen(address, port)) {
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
    if(m_encrypt) {
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
        serverSocket->addCaCertificates("../../certificates/blue_ca.pem");
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

void TCPServer::ParseJsonInput(const QByteArray& buff) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);

    if (doc.isNull()) {
        qDebug() <<"Json error>:" <<error.errorString();
    } else {
        qDebug() << doc.toJson();
    }
}



void TCPServer::Disconnected()
{
    qDebug("Client Disconnected");
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    clientSocket->deleteLater();
}

