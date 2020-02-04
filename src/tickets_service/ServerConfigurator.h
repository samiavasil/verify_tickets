#ifndef SERVERCONFIGURATOR_H
#define SERVERCONFIGURATOR_H

#include<QtGlobal>
#include <QHostAddress>

class QHostAddress;

class ServerConfigurator
{
public:
    ServerConfigurator(QHostAddress address = QHostAddress::Any,
                       quint16 port = 12345,
                       bool encrypted = true);
    virtual ~ServerConfigurator();
    virtual const QHostAddress & getIP();
    virtual quint16 getPort();
    virtual bool is_encrypted();

protected:
    QHostAddress m_address;
    quint16 m_port;
    bool m_encrypted;
};

#endif // SERVERCONFIGURATOR_H
