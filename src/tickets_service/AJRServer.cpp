#include "AJRServer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "db/CodeAccessInfo.h"
#include "db/DeadTickets.h"
#include "db/FiscUnit.h"
#include "db/SiteDescriptor.h"
#include "db/SoldAccess.h"

#define MUSEUM_ID_STR    "mu_id"
#define MSALE_ID_STR      "sale_id"
#define QR_STR           "qr"
#define CODE_STR         "code"
#define CODENAME_STR     "codename"
#define QTY_STR          "qty"

#define GET_VAR_AND_CHECK(jObj, key_id, qvar, type) do { \
    if (!getVal(jObj, key_id, qvar, type)) { \
    qDebug() << "AJR Parser error: Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    }while(0)

#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qDebug() << "Assert error:" << (text) << __func__ << " Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    qDebug() << (text)<< (ok); \
    } \
    while (0)






AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{
    //TODO: Move me

    qDebug() << "Create KeySpace: " << AJRSale::Instance().CreateKeySpace();

    // qDebug() << "Drop Table AJRSale::Instance(): " << AJRSale::Instance().DropTable();
    qDebug() << "Create Table ajrSale: " << AJRSale::Instance().CreateTable();

    QMap<QString, QVariant> result;
    qDebug() << "Dump Table " << AJRSale::Instance().SelectFromTable(QString("*"), result);


    // qDebug() << "Drop Table codeAccessInfo: " << codeAccessInfo.DropTable();
    qDebug() << "Create Table code_Access_info: " << CodeAccessInfo::Instance().CreateTable();
    qDebug() << "Prepare Table code_Access_info: " << CodeAccessInfo::Instance().PrepareCodeAccessTable();

    qDebug() << "Dump Table CodeAccessInfo" << CodeAccessInfo::Instance().SelectFromTable(QString("*"), result);


    DeadTickets deadTickets("test_keyspace_xx", "deadTickets");
    // qDebug() << "Drop Table deadTickets: " << deadTickets.DropTable();
    qDebug() << "Create Table deadTickets: " << deadTickets.CreateTable();


    // qDebug() << "Drop Table fiscUnit: " << fiscUnit.DropTable();
    qDebug() << "Create Table fiscUnit: " << FiscUnit::Instance().CreateTable();
    qDebug() << "PrepareFiscUnitTable: " << FiscUnit::Instance().PrepareFiscUnitTable();

    // qDebug() << "Drop Table siteDescriptor: " << siteDescriptor.DropTable();
    qDebug() << "Create Table SiteDescriptor: " << SiteDescriptor::Instance().CreateTable();
    qDebug() << "PrepareSiteDescriptorTable: " << SiteDescriptor::Instance().PrepareSiteDescriptorTable();

    // qDebug() << "Drop Table soldAccess: " << soldAccess.DropTable();
    qDebug() << "Create Table soldAccess: " << SoldAccess::Instance().CreateTable();

    /*
     * QList<Ajr_data_t> out;
     * QByteArray buf("[{\"mu_id\":1,\"sale_id\":16,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"1016\",\"codename\":\"Единичен. възр.\",\"qty\":\"1\"},{\"mu_id\":1,\"sale_id\":17,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"2323\",\"codename\":\"Нещо си\",\"qty\":\"1\"}]");
     * ParseJsonInput(buf, out);
     */

}

static bool getVal(QJsonObject& jsObj,  const char* field, QVariant& val, QVariant::Type type = QVariant::String) {

    val = QVariant();
    if (jsObj.contains(field)) {
        val = jsObj[field].toVariant();
        if(!val.convert(int(type))) {
            val = QVariant();
        }
        //        qDebug() << field << ": " << val;
    }

    return val.isValid();
}

bool AJRServer::ParseJsonInput(QByteArray& buff, QList<QMap<AJRSale::Column_t , QVariant>> &out) {
    bool status = false;
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(buff, &error);

    bool ok;
    if (doc.isNull()) {
        qDebug() << "Json error:" << error.errorString();
    } else if(!doc.isArray()) {
        qDebug() << tr("Wrong json format: Expected to be arrea type");
    } else {
        out.clear();
        QJsonArray array = doc.array();
        QVariant val;
        QMap<AJRSale::Column_t , QVariant> ajr_data;
        for (int i = 0; i < array.count(); i++) {
            QJsonObject jsObj = array[i].toObject();

            ajr_data.clear();

            GET_VAR_AND_CHECK(jsObj, MUSEUM_ID_STR,val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qDebug() << __func__ << " : Can't get mu_id";
                goto RET_ERROR;
            }
            ajr_data.insert(AJRSale::MU_ID ,val);

            GET_VAR_AND_CHECK(jsObj, MSALE_ID_STR, val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qDebug() << __func__ << " : Can't get sale_id";
                goto RET_ERROR;
            }
            ajr_data.insert(AJRSale::SALE_ID ,val);

            GET_VAR_AND_CHECK(jsObj, QR_STR, val, QVariant::String);
            ajr_data.insert(AJRSale::QR ,val);

            GET_VAR_AND_CHECK(jsObj, CODE_STR, val, QVariant::String);
            ajr_data.insert(AJRSale::CODE ,val);

            GET_VAR_AND_CHECK(jsObj, CODENAME_STR, val, QVariant::String);
            ajr_data.insert(AJRSale::CODENAME ,val);

            GET_VAR_AND_CHECK(jsObj, QTY_STR, val, QVariant::Int);
            val.toInt(&ok);
            if(!ok) {
                qDebug() << __func__ << " : Can't get qty";
                goto RET_ERROR;
            }
            ajr_data.insert(AJRSale::QTY ,val);
            out.append(ajr_data);
        }
        status = true;
    }

RET_ERROR:

    return status;
}

bool AJRServer::ProcessAjurData(QList<QMap<AJRSale::Column_t, QVariant> > &data)
{
    int i;
    bool status = false;

    for (i = 0; i < data.count(); i++) {
        ASSERT_ERROR("AJRSale Insert row: ",
                     AJRSale::Instance().InserRowInSalesTable(data[i]));
        QMap<SoldAccess::Column_t , QVariant> soldData({
                                                    {SoldAccess::MUSEUM_ID, data[i].value(AJRSale::MU_ID, -1)},
                                                    {SoldAccess::SALE_ID, data[i].value(AJRSale::SALE_ID, -1)}
                                                  });
        ASSERT_ERROR("SoldAccess Insert row: ",
                     SoldAccess::Instance().InserRowInSoldAccessTable(soldData));
    }
    if(i > 0) {
        status = true;
    }
RET_ERROR:
    return status;
}

void AJRServer ::Receive()
{
    bool status = false;
    int mu_id   = -1;
    int sale_id = -1;
    QList<QMap<AJRSale::Column_t , QVariant>> data;
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    //Fix me
    QByteArray buf = clientSocket->readAll();

    if(ParseJsonInput(buf, data)) {
        if(ProcessAjurData(data)) {
            status = true;
            mu_id = data[0].value(AJRSale::MU_ID, -1).toInt();//TODO m_lastId initialize on the begining from DB
            sale_id = data[0].value(AJRSale::SALE_ID, -1).toInt();
            m_lastId = mu_id;
        }
    }

    QString result = QString("[{\"mu_id\":%1,\"sale_id\":%2,\"status\":%3}]\n\r").
            arg(mu_id).arg(sale_id).arg(status?1:0);
    clientSocket->write(result.toUtf8().constData());
    //TODO: Fix me
    clientSocket->readAll();
}

