#ifndef SERVERCONFIGURATOR_H
#define SERVERCONFIGURATOR_H

#include<QtGlobal>
#include <QHostAddress>
#include<QString>

class ServerConfigurator
{
public:
    ServerConfigurator(QHostAddress address = QHostAddress::Any,
                       quint16 port = 12345,
                       bool encrypted = true,
                       const QString & key = QString("../../certificates/red_local.key"),
                       const QString & cert = QString("../../certificates/red_local.pem"),
                       const QString & ca = QString("../../certificates/blue_ca.pem")
            );

    ServerConfigurator(const ServerConfigurator& cfg);

    ServerConfigurator &operator =(const ServerConfigurator& cfg);
    virtual ~ServerConfigurator();
    virtual const QHostAddress & getIP();
    virtual quint16 getPort();
    virtual bool is_encrypted();

    QString getKey() const;
    void setKey(const QString &key);

    QString getCert() const;
    void setCert(const QString &cert);

    QString getCa() const;
    void setCa(const QString &ca);

    QHostAddress getAddress() const;
    void setAddress(const QHostAddress &address);

    void setPort(const quint16 &port);

    bool getEncrypted() const;
    void setEncrypted(bool encrypted);

protected:

    QString m_key;
    QString m_cert;
    QString m_ca;
    QHostAddress m_address;
    quint16 m_port;
    bool m_encrypted;
};

#endif // SERVERCONFIGURATOR_H
