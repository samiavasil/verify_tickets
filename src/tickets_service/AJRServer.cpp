#include "AJRServer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#define MUSEUM_ID  "mu_id"
#define SALE_ID    "sale_id"
#define QR         "qr"
#define CODE       "code"
#define CODENAME   "codename"
#define QTY        "qty"

#include "BuildQuerry.h"

AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{
    ParseJsonInput("[{\"mu_id\":1,\"sale_id\":16,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"1016\",\"codename\":\"Единичен. възр.\",\"qty\":\"1\"},{\"mu_id\":1,\"sale_id\":17,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"2323\",\"codename\":\"Нещо си\",\"qty\":\"1\"}]");
}

static bool getVal(QJsonObject& jsObj,  const char* field, QVariant& val, QVariant::Type type = QVariant::String) {

    val = QVariant();
    if (jsObj.contains(field)) {
        val = jsObj[field].toVariant();
        if(!val.convert(int(type))) {
            val = QVariant();
        }
        qDebug() << field << ": " << val;
    }

    return val.isValid();
}

#define GET_VAR_AND_CHECK(jObj, key_id, qvar, type) do { \
    if (!getVal(jObj, key_id, qvar, type)) { \
        qDebug() << "AJR Parser error: Line" << __LINE__; \
        goto RET_ERROR; \
    } \
    }while(0)

AJRServer::DataStatus AJRServer::ParseJsonInput(const QByteArray& buff) {
    DataStatus status = ERROR;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);

    if (doc.isNull()) {
        qDebug() << "Json error:" << error.errorString();
    } else if(!doc.isArray()) {
        qDebug() << tr("Wrong json format: Expected to be arrea type");
    } else {
        qDebug() << "Json Parser: " << doc.toJson();
        QJsonArray array = doc.array();

        for (int i = 0; i < array.count(); i++) {

            QJsonObject jsObj = array[i].toObject();
            QVariant val;
            GET_VAR_AND_CHECK(jsObj, MUSEUM_ID,val, QVariant::Int);
            GET_VAR_AND_CHECK(jsObj, SALE_ID, val, QVariant::String);
            GET_VAR_AND_CHECK(jsObj, QR, val, QVariant::String);
            GET_VAR_AND_CHECK(jsObj, CODE, val, QVariant::String);
            GET_VAR_AND_CHECK(jsObj, CODENAME, val, QVariant::String);
            GET_VAR_AND_CHECK(jsObj, QTY, val, QVariant::Int);
        }
        status = NO_ERROR;
    }

    return status;
RET_ERROR:
    return ERROR;
}

void AJRServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    ParseJsonInput(clientSocket->readAll());
    clientSocket->write("Server says Hello");
}

