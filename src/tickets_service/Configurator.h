#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QSettings>
#include <QString>

class Configurator
{
public:
    typedef struct {
        QString host;
        quint16 port;
        QString user;
        QString password;
        QString feadback_topic;
        QString log_topic;
        bool enbl_logs;
    } MqttCfg_t;

    typedef enum {
        NORMAL,
        DEBUG
    } LogLevel_t;

    static Configurator &Instance();
    int site_id() const;
    int protocol();
    QString hosts();
    QString user();
    QString password();
    QString keyspace();
    bool check_consistency();
    void set_consistency_checked();
    const MqttCfg_t &Mqtt() const;
    bool is_single_consistency_type();
    const LogLevel_t& LogsLevel() const;

protected:
    Configurator();
protected:

    QSettings m_cfg;
    MqttCfg_t  m_Mqtt;
    LogLevel_t m_LogsLevel;
};

#endif // CONFIGURATOR_H
