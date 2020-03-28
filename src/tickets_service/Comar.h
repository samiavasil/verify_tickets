#ifndef COMAR_H
#define COMAR_H

#include <QObject>

#include <QtMqtt/QMqttMessage>
#include <QtMqtt/QMqttSubscription>

class Comar:public QObject
{
    Q_OBJECT
public:
    static Comar& Instance();
    virtual ~Comar();
    const QString hostname();
    void setHostname(const QString &hostname);
    void connectToHost();
    qint32 publish(const QMqttTopicName &topic,
                   const QByteArray &message = QByteArray(),
                   quint8 qos = 0, bool retain = false);
protected:
    Comar(QObject * parent = nullptr,
          const QString &hostname = "127.0.0.1",
          quint16 port = 1883);
protected:
   QMqttClient* m_client;

protected slots:
   void updateLogStateChange();
   void brokerDisconnected();
   void messageReceived(const QMqttMessage &msg);
};

#endif // COMAR_H
