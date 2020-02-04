#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>

class TCPServer: public QTcpServer
{
    Q_OBJECT

public:
    explicit TCPServer(QObject *parent = nullptr,
                       const QHostAddress &address = QHostAddress::Any,
                       quint16 port = 12345,
                       bool encrypt = true);
    virtual ~TCPServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

    QTcpSocket *createSslSocket(qintptr socketDescriptor);
    QTcpSocket *createRegularSocket(qintptr socketDescriptor);
    void ParseJsonInput(const QByteArray &buff);
private slots:

    void EncryptionReady();
    void linkConnection();
    virtual void Receive() = 0;
    void Disconnected();
    void sslErrors(const QList<QSslError> &errors);

private:
    QSslKey m_key;
    QSslCertificate m_cert;
    bool m_encrypt;
};

#endif // TCPSERVER_H
