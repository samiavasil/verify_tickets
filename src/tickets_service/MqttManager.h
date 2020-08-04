#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <QList>
#include <QMap>
#include <QMutex>
#include <QtMqtt/QMqttMessage>
#include <QtMqtt/QMqttSubscription>
#include "Configurator.h"

class MqttManager:public QObject
{
    Q_OBJECT
public:
    typedef QMqttClient* McttClientHndl_t;
    typedef std::function<void(QMqttClient* client, const QByteArray&)> recHndl;
    struct MqttSubscribe_info{
        QString topic;
        quint8 qos;
        recHndl r_hndl;
        friend inline bool operator==(const MqttSubscribe_info& lhs, const MqttSubscribe_info& rhs) {
           return (lhs.topic == rhs.topic) && (lhs.qos == rhs.qos) && (&lhs.r_hndl == &rhs.r_hndl);
        }
    };

    typedef struct MqttSubscribe_info MqttSubscribe_info_t;
    static MqttManager& Instance();
    virtual ~MqttManager();
    const QString hostname();
    McttClientHndl_t createNewMqttClient(const Configurator::MqttCfg_t &mqtt_cfg);
    void setHostname(const QString &hostname);
    void connectMqttClient(const McttClientHndl_t &hndl);
    qint32 publish( const McttClientHndl_t& hndl,
                    const QMqttTopicName &topic,
                    const QByteArray &message = QByteArray(),
                    quint8 qos = 0, bool retain = false);
    static QMqttClient* m_client;
    void subscribe(const McttClientHndl_t hndl, MqttSubscribe_info_t &info);
protected:
    MqttManager(QObject * parent = nullptr);
    void subscribe_int(const McttClientHndl_t hndl, MqttSubscribe_info_t &info);
protected:

   QMap<QMqttClient*, QList<MqttSubscribe_info_t>> m_ClientMap;
   QMutex m_mutex;


protected slots:
   void updateLogStateChange();
   void brockerDisconnected();
};

#endif // MQTT_MANAGER_H
