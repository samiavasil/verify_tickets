#include "AJRServer.h"
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "db/CodeAccessInfo.h"
#include "db/DeadTickets.h"
#include "db/Doors.h"
#include "db/FiscUnit.h"
#include "db/SiteDescriptor.h"
#include "db/SoldAccess.h"
#include "db/SoldAccess.h"

#include<QDebug>

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
    qDebug() << "Error:" << (text) << __func__ << " Line" << __LINE__; \
    goto RET_ERROR; \
    } \
    } \
    while (0)


AJRServer::AJRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

    prepareTables();
#if defined (SIMULATE)
    Receive();
#endif
}

void AJRServer::prepareTables() {
    //TODO: Move me

    //   AJRSale::Instance().setKeySpace("Vesko_Space");

    /* TBD: Should be deprecated - Creating keyspaces and network topology should be
       defined externaly not in aspplication. App will know only the name of keyspace.*/
    /* qDebug() << "Create KeySpace: " << AJRSale::Instance().CreateKeySpace();*/

    //   qDebug() << "Drop Table AJRSale::Instance(): " << AJRSale::Instance().DropTable();
    qDebug() << "Create Table ajrSale: " << AJRSale::Instance().CreateTable();

    QList<QMap<QString, QVariant>> result;
    qDebug() << "Dump Table " << AJRSale::Instance().SelectFromTable(result);

    /*qDebug() << "Drop Table codeAccessInfo: " << CodeAccessInfo::Instance().DropTable();*/
    qDebug() << "Create Table code_Access_info: " << CodeAccessInfo::Instance().CreateTable();
    /*qDebug() << "Prepare Table code_Access_info: " << CodeAccessInfo::Instance().PrepareCodeAccessTable();*/

    qDebug() << "Dump Table CodeAccessInfo" << CodeAccessInfo::Instance().SelectFromTable(result);

    /* qDebug() << "Drop Table deadTickets: " << DeadTickets::Instance().DropTable();*/
    qDebug() << "Create Table deadTickets: " << DeadTickets::Instance().CreateTable();

    /* qDebug() << "Drop Table Doors: " << Doors::Instance().DropTable();*/
    qDebug() << "Create Table Doors: " << Doors::Instance().CreateTable();

    /* qDebug() << "Drop Table fiscUnit: " << FiscUnit::Instance().DropTable(); */
    qDebug() << "Create Table fiscUnit: " << FiscUnit::Instance().CreateTable();
    /* qDebug() << "PrepareFiscUnitTable: " << FiscUnit::Instance().PrepareFiscUnitTable();*/

    /* qDebug() << "Drop Table siteDescriptor: " << SiteDescriptor::Instance().DropTable();*/
    qDebug() << "Create Table SiteDescriptor: " << SiteDescriptor::Instance().CreateTable();
    /*qDebug() << "PrepareSiteDescriptorTable: " << SiteDescriptor::Instance().PrepareSiteDescriptorTable();*/

    /* qDebug() << "Drop Table soldAccess: " << SoldAccess::Instance().DropTable(); */
    qDebug() << "Create Table soldAccess: " << SoldAccess::Instance().CreateTable();
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
            ajr_data.insert(AJRSale::AJ_SITE_ID ,val);

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
            ajr_data.insert(AJRSale::TIMESTAMP_IN ,0);
            ajr_data.insert(AJRSale::TRANSFERED ,false);
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
    }
    if(i > 0) {
        status = true;
    }
RET_ERROR:
    return status;
}

bool AJRServer::TransferSoldAccess(QList<QMap<AJRSale::Column_t, QVariant> > &data) {
    int i;
    bool status = false;
    bool is_ok;
    QList<QMap<QString, QVariant>> code_access;

    for (i = 0; i < data.count(); i++) {
        QMap<SoldAccess::Column_t , QVariant> soldData({
                                                           {SoldAccess::AJ_SITE_ID, data[i].value(AJRSale::AJ_SITE_ID, -1)},
                                                           {SoldAccess::SALE_ID, data[i].value(AJRSale::SALE_ID, -1)},
                                                           /* {SoldAccess::SITE_ID, -1},
                                                                                                              {SoldAccess::DOOR_ID, -1},
                                                                                                              {SoldAccess::USED_CNT, 0},
                                                                                                              {SoldAccess::LIFETIME, 0}, //??
                                                                                                              {SoldAccess::FAIL_OVER_FLAG, false},
                                                                                                              {SoldAccess::TIMESTAMP, 0},*/
                                                       });


        ASSERT_ERROR("SelectFromTable: ",CodeAccessInfo::Instance().
                     SelectFromTable(code_access, "*", QString("code = '%1'").
                                     arg(data[i].value(AJRSale::CODE).toString())));

        ASSERT_ERROR("Code acces should be 1", code_access.count() == 1);

        int access_type = code_access[0].value("access_type").toInt(&is_ok);
        ASSERT_ERROR("Get ACCESS_TYPE", is_ok);
        int dead_level = code_access[0].value("deadlevel").toInt(&is_ok);
        ASSERT_ERROR("Get DEAD_LEVEL", is_ok);

        qDebug() << "ACCESS_TYPE  :"  << access_type;

        if (access_type != CodeAccessInfo::SINGLE) {

            ASSERT_ERROR("Get SIDE_ID", is_ok);
            if (dead_level > 0) {
                QMap<DeadTickets::Column_t , QVariant> deadTicket({
                                                                      {DeadTickets::AJ_SITE_ID, data[0].value(AJRSale::AJ_SITE_ID, -1)},
                                                                      {DeadTickets::SALE_ID, data[0].value(AJRSale::SALE_ID, -1)},
                                                                      {DeadTickets::LIVE_CTR, dead_level},
                                                                  });
                ASSERT_ERROR("SoldAccess Insert row: ",
                             DeadTickets::Instance().InserRowInDeadTickets(deadTicket));
            }

            foreach (auto site_id, code_access[0].value("site_ids").toList()) {
                soldData[SoldAccess::QR_SITE_ID] = site_id.toInt(&is_ok);
                ASSERT_ERROR("Get SIDE_ID", is_ok);
                ASSERT_ERROR("SoldAccess Insert row: ",
                             SoldAccess::Instance().InserRowInSoldAccessTable(soldData));
            }

        }

        data[i][AJRSale::TRANSFERED] = true;
        ASSERT_ERROR("AJRSale Insert row: ",
                     AJRSale::Instance().InserRowInSalesTable(data[i]));

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
    QString result;
    QList<QMap<AJRSale::Column_t , QVariant>> data;

#if defined (SIMULATE)
    QByteArray buf("[{\"mu_id\":1,\"sale_id\":16,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"1016\",\"codename\":\"Единичен. възр.\",\"qty\":\"1\"},{\"mu_id\":1,\"sale_id\":17,\"qr\":\"02496214*19861*2020-01-30*11:01:36*7\",\"code\":\"2323\",\"codename\":\"Нещо си\",\"qty\":\"1\"}]");
#else
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    //Fix me
    QByteArray buf = clientSocket->readAll();

#endif
    ASSERT_ERROR("SoldAccess Insert row: ", ParseJsonInput(buf, data));

    if(ProcessAjurData(data)) {
        status = true;
        m_lastId = data[0].value(AJRSale::AJ_SITE_ID, -1).toInt();
    }

#if !defined (SIMULATE)
    /*Reply to Ajur*/
    result = QString("[{\"mu_id\":%1,\"sale_id\":%2,\"status\":%3}]\n\r").
            arg(m_lastId).arg(data[0].
            value(AJRSale::SALE_ID, -1).toInt()).arg(status?1:0);
    clientSocket->write(result.toUtf8().constData());

#endif
    ASSERT_ERROR("SoldAccess Transfer: ", TransferSoldAccess(data));

RET_ERROR:

#if defined (SIMULATE)
    int a;
#else
    //TODO: Fix me
    clientSocket->readAll();
#endif
}




