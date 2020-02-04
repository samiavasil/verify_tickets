#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>
#include <QSslSocket>
#include <QSslKey>
#include <QSslCertificate>

class SslServer: public QTcpServer
{
    Q_OBJECT

public:
    explicit SslServer(QObject *parent = nullptr,
                       const QHostAddress &address = QHostAddress::Any,
                       quint16 port = 12345,
                       bool bencr = true);
    virtual ~SslServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

private slots:

    void EncriptionReady();
    void linkConnection();
    void Receive();
    void Disconnected();
    void sslErrors(const QList<QSslError> &errors);

private:
    QSslKey m_key;
    QSslCertificate m_cert;
};

#endif // SSLSERVER_H
