#include "AJRServer.h"
#include<QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}


static QVariant getVal(QJsonObject& jsObj,  const char* field, QVariant::Type type = QVariant::String) {
    QVariant val;
    if(jsObj.contains(field) && jsObj[field].toVariant().canConvert(type)) {
        val = jsObj[field].toVariant();
        qDebug() << field << ": " << val.toString();
    }
    return val;
}

void AJRServer::ParseJsonInput(const QByteArray& buff) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);
    qDebug() << buff;
    if (doc.isNull()) {
        qDebug() <<"Json error>:" <<error.errorString();
    } else if(!doc.isArray()) {
        qDebug() << tr("Wrong json format: Expected to be arrea type");
    } else {

        qDebug() << "Json Parser: " << doc.toJson();
        QJsonArray array = doc.array();
        for (int i = 0; i < array.count(); i++) {
            QJsonObject jsObj = array[i].toObject();

            QVariant val = getVal(jsObj, "mu_id",QVariant::Int);
            val = getVal(jsObj, "sel_id",QVariant::Int);
            val = getVal(jsObj, "qr",QVariant::String);
            val = getVal(jsObj, "code",QVariant::String);
            val = getVal(jsObj, "codename",QVariant::String);
            val = getVal(jsObj, "qty",QVariant::Int);
        }
    }
}

void AJRServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    ParseJsonInput(clientSocket->readAll());
    clientSocket->write("Server says Hello");
}
