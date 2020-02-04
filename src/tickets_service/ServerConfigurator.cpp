#include "ServerConfigurator.h"

/*!
 * \brief ServerConfigurator::ServerConfigurator
 * \param address
 * \param port
 * \param encrypted
 */
ServerConfigurator::ServerConfigurator(QHostAddress address, quint16 port,
                                       bool encrypted):m_address(address),
                                       m_port(port), m_encrypted(encrypted)
{

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
