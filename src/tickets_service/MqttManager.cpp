#include "MqttManager.h"
#include <QDebug>
#include <QtMqtt/QMqttClient>
#include "Configurator.h"

MqttManager &MqttManager::Instance()
{
    static MqttManager sMqtt(nullptr, Configurator::Instance().Mqtt().host, Configurator::Instance().Mqtt().port);
    return sMqtt;
}

MqttManager::MqttManager(QObject *parent,
             const QString &hostname,
             quint16 port) : QObject(parent)
{
    m_client = new QMqttClient(this);
    m_client->setHostname(hostname);
    m_client->setPort(port);
    connect(m_client, &QMqttClient::stateChanged, this, &MqttManager::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &MqttManager::brokerDisconnected);

    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" Received Topic: ")
                + topic.name()
                + QLatin1String(" Message: ")
                + message
                + QLatin1Char('\n');
        qDebug() << this << content;
    });

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
                + QLatin1String(" PingResponse")
                + QLatin1Char('\n');
        qDebug() << this << content;
    });

    m_client->setUsername(Configurator::Instance().Mqtt().user);
    m_client->setPassword(Configurator::Instance().Mqtt().password);
}


MqttManager::~MqttManager()
{

}

const QString MqttManager::hostname()
{
    return m_client->hostname();
}

void MqttManager::setHostname(const QString &hostname)
{
    m_client->setHostname(hostname);
}

void MqttManager::connectToHost()
{
    m_client->connectToHost();
}

qint32 MqttManager::publish(const QMqttTopicName &topic, const QByteArray &message,
                      quint8 qos, bool retain) {
    return m_client->publish(topic, message, qos, retain);
}

void MqttManager::updateLogStateChange()
{
    auto state = m_client->state();
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(": State Change")
            + QString::number(state)
            + QLatin1Char('\n');

    qDebug() << content;

    if(state == m_client->Connected) {
        auto subscription = m_client->subscribe(
                    QMqttTopicFilter(Configurator::Instance().Mqtt().feadback_topic), 0);
        if (!subscription) {
            qCritical() << "Error: Could not subscribe."
                        " Is there a valid connection?";
            return;
        }
        connect(subscription, &QMqttSubscription::messageReceived, this, &MqttManager::messageReceived);

        connect(subscription, &QMqttSubscription::qosChanged, [this](quint8 qos) {
            qDebug() << this << "QOS changed to: " << QString::number(qos);
        });

        connect(subscription, &QMqttSubscription::stateChanged,
                this, [subscription](QMqttSubscription::SubscriptionState state){
            switch (state) {
            case QMqttSubscription::Unsubscribed:
                qDebug() << "Unsubscribed";
                subscription->deleteLater();
                break;
            case QMqttSubscription::SubscriptionPending:
                qDebug() <<"Pending";
                break;
            case QMqttSubscription::Subscribed:
                qDebug() <<"Subscribed";
                break;
            case QMqttSubscription::Error:
                qCritical() <<"Error";
                break;
            default:
                qCritical() <<"--Unknown--";
                break;
            }
        });
    }
}

void MqttManager::brokerDisconnected()
{
    qDebug() << "Disconnected";
}


void MqttManager::messageReceived(const QMqttMessage &msg)
{
    qDebug() << msg.payload();
}

