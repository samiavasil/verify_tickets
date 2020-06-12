#include "AJRServer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>

#include "Configurator.h"
#include "db/DBClient.h"


#define MUSEUM_ID_STR    "mu_id"
#define MSALE_ID_STR     "sale_id"
#define QR_STR           "qr"
#define CODE_STR         "code"
#define CODENAME_STR     "codename"
#define QTY_STR          "qty"

#define GET_VAR_AND_CHECK(jObj, key_id, qvar, type) do { \
    if (!getVal(jObj, key_id, qvar, type)) { \
    qCritical() << "AJR Parser error: Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    }while(0)

#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qCritical() << "Error:" << (text) << __func__ << " Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    } \
    while (0)

static bool getVal(QJsonObject& jsObj,  const char* field, QVariant& val, QVariant::Type type = QVariant::String) {

    val = QVariant();
    if (jsObj.contains(field)) {
        val = jsObj[field].toVariant();
        if(!val.convert(int(type))) {
            val = QVariant();
        }
    }

    return val.isValid();
}

/**
 * @brief AJRServer::AJRServer
 * @param parent
 * @param config
 */
AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{
}

/**
 * @brief AJRServer::ParseJsonInput
 * @param buff
 * @param out
 * @return
 */
bool AJRServer::ParseJsonInput(QByteArray& buff, QList<QMap<QString , QVariant>> &out) {
    bool status = false;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);

    bool ok;
    if (doc.isNull()) {
        qWarning() << "Json error:" << error.errorString();
    } else if(!doc.isArray()) {
        qWarning() << tr("Wrong json format: Expected to be arrea type %1");
    } else {
        out.clear();
        QJsonArray array = doc.array();
        QVariant val;
        QMap<QString , QVariant> ajr_data;
        for (int i = 0; i < array.count(); i++) {
            QJsonObject jsObj = array[i].toObject();

            ajr_data.clear();

            GET_VAR_AND_CHECK(jsObj, MUSEUM_ID_STR,val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qWarning() << __func__ << " : Can't get mu_id";
                goto RET_ERROR;
            }
            ajr_data.insert("aj_site_id" ,val);

            GET_VAR_AND_CHECK(jsObj, MSALE_ID_STR, val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qWarning() << __func__ << " : Can't get sale_id";
                goto RET_ERROR;
            }
            ajr_data.insert("sale_id" ,val);

            GET_VAR_AND_CHECK(jsObj, QR_STR, val, QVariant::String);
            ajr_data.insert("qr" ,val);

            GET_VAR_AND_CHECK(jsObj, CODE_STR, val, QVariant::String);
            ajr_data.insert("code" ,val);

            GET_VAR_AND_CHECK(jsObj, CODENAME_STR, val, QVariant::String);
            ajr_data.insert("codename" ,val);

            GET_VAR_AND_CHECK(jsObj, QTY_STR, val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qWarning() << __func__ << " : Can't get qty";
                goto RET_ERROR;
            }
            ajr_data.insert("qty", val);
            ajr_data.insert("timestamp_in", QDateTime::currentDateTimeUtc());
            ajr_data.insert("transfered", false);
            out.append(ajr_data);
        }
        status = true;
    }
RET_ERROR:

    return status;
}

/**
 * @brief AJRServer::ProcessAjurData
 * @param data
 * @return
 */
bool AJRServer::ProcessAjurData(QList<QMap<QString, QVariant> > &data)
{
    return DBClient::Instance().InsertRowsInAjrSale(data);
}

/**
 * @brief AJRServer::Receive
 */
void AJRServer ::Receive()
{
    bool status = false;
    QString result;
    QList<QMap<QString , QVariant>> data;
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    int error_code = 0;
    //Fix me
    QByteArray buf = clientSocket->readAll();

    ASSERT_ERROR("Parse Ajure Json input: ", ParseJsonInput(buf, data));
    ASSERT_ERROR("Process Ajur Data: ", ProcessAjurData(data));

    if (DBClient::Instance().connState() == DBClient::CONNECTED) {
        if (DBClient::Instance().TransferSoldAccess(data))
        {
            status = true;
            m_lastId = data[0].value("aj_site_id", -1).toInt();
            qInfo() << "New ajure sale: " << data;
        } else {
            error_code = 1;
            qInfo() << "Ajure sale error: Can't write to Cassandra DB " << error_code;
        }

    } else {
        //Failover Logic here
        //Add reason for fail
        error_code = 2;
        qInfo() << "AjrSrv: Failover state: " <<  error_code;
    }

RET_ERROR:
    /*Reply to Ajur*/
    result = QString("[{\"mu_id\":%1,\"sale_id\":%2,\"status\":%3,\"error_code\":%4}]\n\r").
            arg(m_lastId).
            arg(data[0].value("sale_id", -1).toInt()).
            arg(status?1:0).
            arg(error_code);

    clientSocket->write(result.toUtf8().constData());
    clientSocket->readAll();
}
