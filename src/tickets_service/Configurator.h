#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QSettings>
#include <QString>

typedef struct {
    QString host;
    quint16 port;
    QString user;
    QString password;
    QString feadback;
} MqttCfg;

class Configurator
{
public:
    static Configurator &Instance();
    int site_id() const;
    int protocol();
    QString hosts();
    QString user();
    QString password();
    QString keyspace();
    bool check_consistency();
    void set_consistency_checked();
    const MqttCfg &Mqtt() const;
protected:
    Configurator();
protected:
    QSettings m_cfg;
    MqttCfg  m_Mqtt;
};

#endif // CONFIGURATOR_H
