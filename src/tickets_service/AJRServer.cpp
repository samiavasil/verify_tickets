#include "AJRServer.h"
#include<QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}

void AJRServer::ParseJsonInput(const QByteArray& buff) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);

    if (doc.isNull()) {
        qDebug() <<"Json error>:" <<error.errorString();
    } else if(!doc.isArray()) {
        qDebug() << tr("Wrong json format: Expected to be arrea type");
    } else {

        qDebug() << "Json Parser";//doc.toJson();
        QJsonArray array = doc.array();
        for (int i = 0; i < array.count(); i++) {
            QJsonObject jsObj = array[i].toObject();

            if(jsObj.contains("sale_id") && jsObj["sale_id"].isString()) {
                qDebug() << "sale_id: " << jsObj["sale_id"].toString();
            }

            if(jsObj.contains("qr") && jsObj["qr"].isString()) {
                qDebug() << "qr: " << jsObj["qr"].toString();
            }

            if(jsObj.contains("code")) {
                int code = jsObj.value("code").toInt(-1);
                if(code > 0) {
                    qDebug() << "code: " << code;
                }
            }

            if(jsObj.contains("name") && jsObj["name"].isString()) {
                qDebug() << "name: " << jsObj["name"].toString();
            }

            if(jsObj.contains("qty")) {
                if(jsObj.contains("qty")) {
                    int qty = jsObj.value("qty").toInt(-1);
                    if(qty > 0) {
                        qDebug() << "qty: " << qty;
                    }
                }
            }
        }
    }
}

void AJRServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    ParseJsonInput(clientSocket->readAll());
    clientSocket->write("Server says Hello");
}
