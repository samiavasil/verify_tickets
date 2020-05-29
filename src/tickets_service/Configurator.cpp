#include "Configurator.h"
#include <iostream>
#include <string>
#include <QDebug>

#define CASSANDRA_DEFAULT_VERSION    4
#define CASSANDRA_DEFAULT_HOSTS      "192.168.4.221,192.168.4.222, 192.168.2.221,192.168.2.222"
#define CASSANDRA_DEFAULT_USER       "cassandra"
#define CASSANDRA_DEFAULT_PASSWORD   "cassandra"
#define CASSANDRA_DEFAULT_KEYSPACE   "test_keyspace_xx"

#define CASSANDRA_DEFAULT_MQTT_HOST "87.97.172.156"
#define CASSANDRA_DEFAULT_MQTT_PORT 7328
#define CASSANDRA_DEFAULT_MQTT_USER "mqtt"
#define CASSANDRA_DEFAULT_MQTT_PASS "778899123Abv!"
#define CASSANDRA_DEFAULT_MQTT_FB_TOPIC "Museum/Feadback"
#define CASSANDRA_DEFAULT_CONSISTENCY "Ako sakash da aktivirash proverka na DB consistenciata, set his value to: check"

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
    QVariant value;
    m_cfg.setIniCodec("UTF-8");

    if (m_cfg.value("site_config/site_id").isNull()) {
        m_cfg.setValue("site_config/site_id", 10);
    }
    bool debug = m_cfg.value("site_config/debug", false).toBool();

    value = m_cfg.value("mqtt_config/host");
    if (value.isNull()) {
        value = CASSANDRA_DEFAULT_MQTT_HOST;
        m_cfg.setValue("mqtt_config/host", value);
    }
    m_Mqtt.host = value.toString();

    value = m_cfg.value("mqtt_config/port");
    if (value.isNull()) {
        value = CASSANDRA_DEFAULT_MQTT_PORT;
        m_cfg.setValue("mqtt_config/port", value);
    }
    m_Mqtt.port = static_cast<quint16>(value.toInt());

    value = m_cfg.value("mqtt_config/user");
    if (value.isNull()) {
        value = CASSANDRA_DEFAULT_MQTT_USER;
        m_cfg.setValue("mqtt_config/user", CASSANDRA_DEFAULT_MQTT_USER);
    }
    m_Mqtt.user = value.toString();

    value = m_cfg.value("mqtt_config/pass");
    if (value.isNull()) {
        value = CASSANDRA_DEFAULT_MQTT_PASS;
        m_cfg.setValue("mqtt_config/pass", CASSANDRA_DEFAULT_MQTT_PASS);
    }
    m_Mqtt.password = value.toString();

    value = m_cfg.value("mqtt_config/feadback");
    if (value.isNull()) {
        value = CASSANDRA_DEFAULT_MQTT_FB_TOPIC;
        m_cfg.setValue("mqtt_config/feadback", CASSANDRA_DEFAULT_MQTT_FB_TOPIC);
    }
    m_Mqtt.feadback = value.toString();
}

const MqttCfg &Configurator::Mqtt() const
{
    return m_Mqtt;
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

QString Configurator::keyspace()
{
    QVariant keyspace = m_cfg.value("cassandra/keyspace");
    if (keyspace.isNull()) {
        m_cfg.setValue("cassandra/keyspace", CASSANDRA_DEFAULT_KEYSPACE);
        keyspace = CASSANDRA_DEFAULT_KEYSPACE;
    }

    return keyspace.toString();
}

bool Configurator::check_consistency()
{
    bool check_consistency = false;
    QVariant consitency = m_cfg.value("cassandra/consitency");

    if (consitency.isNull()) {
       m_cfg.setValue("cassandra/consitency", CASSANDRA_DEFAULT_CONSISTENCY);
    } else {
        if(!QString::compare(consitency.toString(), "check", Qt::CaseInsensitive)) {
            check_consistency = true;
        }
    }
    return check_consistency;
}

void Configurator::set_consistency_checked()
{
    m_cfg.setValue("cassandra/consitency", "checked");
}
