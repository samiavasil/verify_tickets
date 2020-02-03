#ifndef SSLSERVER_H
#define SSLSERVER_H

#include <QTcpServer>

class SslServer: public QTcpServer
{
    Q_OBJECT
public:
    explicit SslServer(QObject *parent = nullptr);
    virtual ~SslServer();

protected:
    virtual void incomingConnection(qintptr socketDescriptor);

protected slots:
    void ready();
};

#endif // SSLSERVER_H
