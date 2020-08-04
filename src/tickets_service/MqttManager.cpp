#include "MqttManager.h"
#include <QDebug>
#include <QtMqtt/QMqttClient>

QMqttClient* MqttManager::m_client;
MqttManager &MqttManager::Instance()
{
    static MqttManager sMqtt(nullptr);
    return sMqtt;
}

MqttManager::MqttManager(QObject *parent) : QObject(parent)
{
    /*
        TBD: fix me
    */
    m_client = createNewMqttClient(Configurator::Instance().Mqtt());
}


MqttManager::~MqttManager()
{

}

const QString MqttManager::hostname()
{
    return m_client->hostname();
}

MqttManager::McttClientHndl_t MqttManager::createNewMqttClient(const Configurator::MqttCfg_t &mqtt_cfg)
{
    McttClientHndl_t client = new QMqttClient(this);
    client->setHostname(mqtt_cfg.host);
    client->setPort(mqtt_cfg.port);
    connect(client, &QMqttClient::stateChanged, this, &MqttManager::updateLogStateChange);
    connect(client, &QMqttClient::disconnected, this, &MqttManager::brockerDisconnected);
    connect(client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
        qDebug() << this << content;
    });

    client->setUsername(mqtt_cfg.user);
    client->setPassword(mqtt_cfg.password);
    m_mutex.lock();
    m_ClientMap.insert(client, QList<MqttSubscribe_info_t>());
    m_mutex.unlock();

    return client;
}

void MqttManager::setHostname(const QString &hostname)
{
    m_client->setHostname(hostname);
}

void MqttManager::connectMqttClient(const McttClientHndl_t &hndl)
{
    hndl->connectToHost();
}

qint32 MqttManager::publish(const McttClientHndl_t &hndl, const QMqttTopicName &topic, const QByteArray &message,
                            quint8 qos, bool retain) {
    return hndl->publish(topic, message, qos, retain);
}

void MqttManager::subscribe(const McttClientHndl_t hndl, MqttSubscribe_info_t &info) {
    m_mutex.lock();
    subscribe_int(hndl, info);
    m_mutex.unlock();
}

void MqttManager::subscribe_int(const McttClientHndl_t hndl, MqttSubscribe_info_t &info) {

    QMqttClient* client =  hndl;
    if (!m_ClientMap.contains(client)) {
        qInfo() << "Mqtt not existing client handle";
    }

    if (!m_ClientMap[client].contains(info)) {
         qDebug() << "Mqtt client: " << client << " - added subscription to topic: '" << info.topic << "'" ;
         m_ClientMap[client].append(info);
    }

    auto subscription = client->subscribe(QMqttTopicFilter(info.topic), info.qos);
    if (!subscription) {

        qCritical() << "Error: Could not subscribe."
                       " Is there a valid connection?";
        return;
    }

    if (subscription->state() == subscription->Subscribed) {
        qDebug() << "Mqtt client already connected to topic: '" << info.topic << "'" ;
        return;
    }

    connect(subscription, &QMqttSubscription::messageReceived, this, [client, info](QMqttMessage msg) {
        if (info.r_hndl != nullptr) {
            info.r_hndl(client, msg.payload());
        }
    });

    connect(subscription, &QMqttSubscription::qosChanged, [this](quint8 qos) {
        qDebug() << this << "QOS changed to: " << QString::number(qos);
    });

    connect(subscription, &QMqttSubscription::stateChanged,
            this, [subscription](QMqttSubscription::SubscriptionState state){
        switch (state) {
        case QMqttSubscription::Unsubscribed:
            qDebug() << "Unsubscribed: topic - '" << subscription->topic() << "'";
            subscription->deleteLater();
            break;
        case QMqttSubscription::SubscriptionPending:
            qDebug() <<"Pending: topic '" << subscription->topic() << "'";
            break;
        case QMqttSubscription::Subscribed:
            qDebug() <<"Subscribed:  topic '" << subscription->topic() << "'";
            break;
        case QMqttSubscription::Error:
            qCritical() << "Error: topic - '"  << subscription->topic()
                        << "', Reason: " << subscription->reason();
            break;
        default:
            qCritical() <<"--Unknown--";
            break;
        }
    });
}

void MqttManager::updateLogStateChange()
{
    QMqttClient* client = qobject_cast<QMqttClient*>(sender());
    auto state = client->state();
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(": State Change")
            + QString::number(state)
            + QLatin1Char('\n');

    qDebug() << content;
    if(state == client->Connected) {
        foreach (auto info, m_ClientMap.value(client)) {
            subscribe_int(client, info);
        }
    }
}

void MqttManager::brockerDisconnected()
{
    QMqttClient* client = qobject_cast<QMqttClient*>(sender());

    qDebug() << "Disconnected: Host "  << client->hostname()
             << ", Port " << client->port();
}


