#include "ServerConfigurator.h"

ServerConfigurator::ServerConfigurator(QHostAddress address,
                                       quint16 port,
                                       bool encrypted,
                                       const QString & key,
                                       const QString & cert,
                                       const QString & ca)
{
    m_key = key;
    m_cert = cert;
    m_ca = ca;
    m_address = address;
    m_port = port;
    m_encrypted = encrypted;
}

/**
 * @brief ServerConfigurator::ServerConfigurator
 * @param cfg
 */
ServerConfigurator::ServerConfigurator(const ServerConfigurator &cfg)
{
    m_key       = cfg.m_key       ;
    m_cert      = cfg.m_cert      ;
    m_ca        = cfg.m_ca        ;
    m_address   = cfg.m_address   ;
    m_port      = cfg.m_port      ;
    m_encrypted = cfg.m_encrypted ;
}

/**
 * @brief ServerConfigurator::operator =
 * @param cfg
 * @return
 */
ServerConfigurator& ServerConfigurator::operator =(const ServerConfigurator &cfg)
{
    if (this != &cfg) {
        m_key       = cfg.m_key       ;
        m_cert      = cfg.m_cert      ;
        m_ca        = cfg.m_ca        ;
        m_address   = cfg.m_address   ;
        m_port      = cfg.m_port      ;
        m_encrypted = cfg.m_encrypted ;
    }
    return *this;
}

/*!
 * \brief ServerConfigurator::~ServerConfigurator
 */
ServerConfigurator::~ServerConfigurator()
{

}

/*!
 * \brief ServerConfigurator::getIP
 * \return
 */
const QHostAddress &ServerConfigurator::getIP()
{
    return m_address;
}

/*!
 * \brief ServerConfigurator::getPort
 * \return
 */
quint16 ServerConfigurator::getPort()
{
    return m_port;
}

/*!
 * \brief ServerConfigurator::is_encrypted
 * \return
 */
bool ServerConfigurator::is_encrypted()
{
    return m_encrypted;
}

/**
 * @brief ServerConfigurator::getKey
 * @return
 */
QString ServerConfigurator::getKey() const
{
    return m_key;
}

/**
 * @brief ServerConfigurator::setKey
 * @param key
 */
void ServerConfigurator::setKey(const QString &key)
{
    m_key = key;
}

/**
 * @brief ServerConfigurator::getCert
 * @return
 */
QString ServerConfigurator::getCert() const
{
    return m_cert;
}

/**
 * @brief ServerConfigurator::setCert
 * @param cert
 */
void ServerConfigurator::setCert(const QString &cert)
{
    m_cert = cert;
}

/**
 * @brief ServerConfigurator::getCa
 * @return
 */
QString ServerConfigurator::getCa() const
{
    return m_ca;
}

/**
 * @brief ServerConfigurator::setCa
 * @param ca
 */
void ServerConfigurator::setCa(const QString &ca)
{
    m_ca = ca;
}

/**
 * @brief ServerConfigurator::getAddress
 * @return
 */
QHostAddress ServerConfigurator::getAddress() const
{
    return m_address;
}

/**
 * @brief ServerConfigurator::setAddress
 * @param address
 */
void ServerConfigurator::setAddress(const QHostAddress &address)
{
    m_address = address;
}

/**
 * @brief ServerConfigurator::setPort
 * @param port
 */
void ServerConfigurator::setPort(const quint16 &port)
{
    m_port = port;
}

/**
 * @brief ServerConfigurator::getEncrypted
 * @return
 */
bool ServerConfigurator::getEncrypted() const
{
    return m_encrypted;
}

/**
 * @brief ServerConfigurator::setEncrypted
 * @param encrypted
 */
void ServerConfigurator::setEncrypted(bool encrypted)
{
    m_encrypted = encrypted;
}
