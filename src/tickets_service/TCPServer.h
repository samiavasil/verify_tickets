#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>
#include "ServerConfigurator.h"

class TCPServer: public QTcpServer
{
    Q_OBJECT

public:
    explicit TCPServer(QObject *parent,   const ServerConfigurator &config);
    virtual ~TCPServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

    QTcpSocket *createSslSocket(qintptr socketDescriptor);
    QTcpSocket *createRegularSocket(qintptr socketDescriptor);

private slots:

    void EncryptionReady();
    void linkConnection();
    virtual void Receive() = 0;
    void Disconnected();
    void sslErrors(const QList<QSslError> &errors);

private:
    QSslKey m_key;
    QSslCertificate m_cert;
    ServerConfigurator m_config;
};

#endif // TCPSERVER_H
