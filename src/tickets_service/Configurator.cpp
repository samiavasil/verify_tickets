#include "Configurator.h"
#include <iostream>
#include <string>
#include <QDebug>

#define CASSANDRA_DEFAULT_VERSION 4
#define CASSANDRA_DEFAULT_HOSTS     "192.168.4.221,192.168.4.222, 192.168.2.221,192.168.2.222"
#define CASSANDRA_DEFAULT_USER      "cassandra"
#define CASSANDRA_DEFAULT_PASSWORD  "cassandra"

using namespace std;
Configurator &Configurator::Instance()
{
    static Configurator m_inst;
    return m_inst;
}

int Configurator::site_id() const
{
    return m_cfg.value("site_config/site_id", -1).toInt();
}

Configurator::Configurator():m_cfg("./config.ini", QSettings::IniFormat)
{
    m_cfg.setIniCodec("UTF-8");
    //m_cfg.setValue("animal/snake", 58);

    if (m_cfg.value("site_config/site_id").isNull()) {
        m_cfg.setValue("site_config/site_id", 10);
    }

    int site_id = m_cfg.value("site_config/site_id", -1).toInt();

    bool debug = m_cfg.value("site_config/debug", false).toBool();
    qDebug() << "site_id: "<< site_id << ", debug: " << debug;
    //   string p;
    //   cin >> p;
}

int Configurator::protocol() {
    QVariant proto = m_cfg.value("cassandra/protocol");
    int protocol = -1;
    bool ok;

    if (proto.isNull()) {
        m_cfg.setValue("cassandra/protocol", CASSANDRA_DEFAULT_VERSION);
        proto = CASSANDRA_DEFAULT_VERSION;
    }
    protocol = proto.toInt(&ok);
    if (!ok) {
        qDebug() << "Can't read cassandra protocol version";
    }
    return protocol;
}

QString Configurator::hosts()
{
    QVariant hosts = m_cfg.value("cassandra/hosts");
    if (hosts.isNull()) {
        m_cfg.setValue("cassandra/hosts", CASSANDRA_DEFAULT_HOSTS);
        hosts = CASSANDRA_DEFAULT_HOSTS;
    }

    return hosts.toString();
}

QString Configurator::user()
{
    QVariant user = m_cfg.value("cassandra/user");
    if (user.isNull()) {
        m_cfg.setValue("cassandra/user", CASSANDRA_DEFAULT_USER);
        user = CASSANDRA_DEFAULT_USER;
    }

    return user.toString();
}

QString Configurator::password()
{
    QVariant password = m_cfg.value("cassandra/password");
    if (password.isNull()) {
        m_cfg.setValue("cassandra/password", CASSANDRA_DEFAULT_PASSWORD);
        password = CASSANDRA_DEFAULT_PASSWORD;
    }

    return password.toString();
}
