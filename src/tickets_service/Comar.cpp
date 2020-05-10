#include "Comar.h"
#include <QDebug>
#include <QtMqtt/QMqttClient>
#include "Configurator.h"

Comar &Comar::Instance()
{
    static Comar sComar(nullptr, Configurator::Instance().Comar().host, Configurator::Instance().Comar().port);
    return sComar;
}

Comar::Comar(QObject *parent,
             const QString &hostname,
             quint16 port) : QObject(parent)
{
    m_client = new QMqttClient(this);
    m_client->setHostname(hostname);
    m_client->setPort(port);
    connect(m_client, &QMqttClient::stateChanged, this, &Comar::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &Comar::brokerDisconnected);

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

    m_client->setUsername(Configurator::Instance().Comar().user);
    m_client->setPassword(Configurator::Instance().Comar().password);
}


Comar::~Comar()
{

}

const QString Comar::hostname()
{
    return m_client->hostname();
}

void Comar::setHostname(const QString &hostname)
{
    m_client->setHostname(hostname);
}

void Comar::connectToHost()
{
    m_client->connectToHost();
}

qint32 Comar::publish(const QMqttTopicName &topic, const QByteArray &message,
                      quint8 qos, bool retain) {
    return m_client->publish(topic, message, qos, retain);
}

void Comar::updateLogStateChange()
{
    auto state = m_client->state();
    const QString content = QDateTime::currentDateTime().toString()
            + QLatin1String(": State Change")
            + QString::number(state)
            + QLatin1Char('\n');

    qDebug() << content;

    if(state == m_client->Connected) {
        auto subscription = m_client->subscribe(
                    QMqttTopicFilter(Configurator::Instance().Comar().topic), 0);
        if (!subscription) {
            qDebug() << "Error: Could not subscribe."
                        " Is there a valid connection?";
            return;
        }
        connect(subscription, &QMqttSubscription::messageReceived, this, &Comar::messageReceived);

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
                qDebug() <<"Error";
                break;
            default:
                qDebug() <<"--Unknown--";
                break;
            }
        });
    }
}

void Comar::brokerDisconnected()
{
    qDebug() << "Disconnected";
}


void Comar::messageReceived(const QMqttMessage &msg)
{
    qDebug() << msg.payload();
}

