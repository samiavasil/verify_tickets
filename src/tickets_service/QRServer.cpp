#include "QRServer.h"
#include "db/DBClient.h"
#include "MqttManager.h"
#include <QJsonDocument>
#include <qjsonobject.h>
#include "Configurator.h"

/**
* @brief GOTO_EXIT: goto statement
* @param EXIT goto point
* @param LOG  qDebug, qInfo, qCritical - log levels
* @param STR  SAtring to log
*/
#define GOTO_EXIT(EXIT, LOG, STR) do {\
    codename = (STR);\
    LOG() << "Verifivation error: " << (STR);\
    goto EXIT;\
    } while(0)\


/**
 *TDB : Not used for now
 */
enum {
    E_OK,
    E_QR_BAD_FORMAT_SITE_ID,
    E_QR_BAD_FORMAT_DOOR_ID,
    E_QR_BAD_FORMAT,
    E_DB_ACCESS_ERROR,
    E_BAD_BARCODE_FORMAT,
    INVLAID_QR_CODE,
};


QRServer::QRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}

QRServer::~QRServer()
{

}


/**
 * @brief QRServer::ParseQrCode
 * @param qr
 * @param qr_site_id
 * @param door_id
 * @param codename
 * @return
 */
bool QRServer::ParseQrCode(QString& qr, int &qr_site_id, int &door_id, QString& codename) {

    QStringList split_site_door;
    bool ok;
    bool ret = false;

    split_site_door = qr.split(":");

    if (split_site_door.count() >= 3 ) {
        qr_site_id = split_site_door[0].toInt(&ok);
        if (!ok) {
            codename = "QR Code bad format: qr_site_id";
            goto EXIT;
        }

        qr.remove(0, split_site_door[0].length() + 1);
        /*Get door ID*/
        door_id = split_site_door[1].toInt(&ok);
        if (!ok) {
            codename = "QR Code bad format: door_id";
            goto EXIT;
        }
        qr.remove(0, split_site_door[1].count() + 1);
        ret = true;
    } else {
        codename = " Not correct QR format (qr_site_id) should be: "
                   "qr_site_id:door_id:QR";
        goto EXIT;
    }

EXIT:
    return ret;
}

bool QRServer::CheckFiscalUnits(QString &qr, QList<QMap<QString, QVariant>> &fisc_unit,
                                QRServer::qr_t &qr_bar, QString &codename)
{
    QStringList qr_list;
    bool ret = false;

    qr_list = qr.split("*");

    if (qr_list.count() > 0) {
        if (qr_list.count() > 1) {
            if(!DBClient::Instance().FindFiscalMem(fisc_unit, qr_list[0])) {
                codename = "Can't get fiscal mem";
                goto EXIT;
            }
            qDebug() << "Продадено от фискален апарат" << fisc_unit[0].value("fiscal_mem", "Undefined");
            qr_bar = QR;
            ret = true;
        } else {
            /*TBD: logic for Barcodes here*/
            qCritical() << "TBD: BAR CODE UNIT CHECK ISN'T IMPLEMENTED";
            qr_bar = BARCODE;
            ret = true;
        }

    } else {
        codename = " Not correct QR format (qr_site_id) should be: "
                   "qr_site_id:door_id:QR";
    }

EXIT:

    return ret;
}

bool QRServer::GetAjrSales(QList<QMap<QString, QVariant>> &ajr_sales, QString &qr, QString &codename) {

    bool ret = DBClient::Instance().GetAjrSales(ajr_sales, "*",
                                                QString("qr='%1' ALLOW FILTERING").
                                                arg(qr));
    if (ret) {
        if (ajr_sales.count() <= 0) {
            ret = false;
            codename = "Not valid QR code";
            goto EXIT;
        }

        if (!ajr_sales[0].value("transfered").toBool()) {
            if(!DBClient::Instance().TransferSoldAccess(ajr_sales)) {
                ret = false;
                codename = "Can't transfered Ajure sale.";
                goto EXIT;
            }
            qDebug() << "Transfered";
        }
    }
EXIT:

    return ret;
}

bool QRServer::GetSoldAccesForSale(QList<QMap<QString, QVariant>> &sold_access,
                                   int &sale_id, int &aj_site_id, QString &code) {
    return DBClient::Instance().GetSoldAccess(sold_access, "*",
                                              QString("sale_id = %1"
                                                      " and aj_site_id = %2"
                                                      " and code = '%3'"
                                                      " ALLOW FILTERING").
                                              arg(sale_id).
                                              arg(aj_site_id).
                                              arg(code));
}

bool QRServer::IsSingleAccessTypeEnabled(const QList<QMap<QString, QVariant>> &sold_access,
                                         const   QVariantList  &site_ids, const int &qr_site_id) {

    bool enable_access = false;
    bool already_used = false;
    QString codename;
    bool ok;

    foreach (auto sold, sold_access) {
        int f_qr_site_id = sold.value("qr_site_id").toInt(&ok);
        if (!ok) {
            GOTO_EXIT(EXIT, qDebug, "Can't get qr_site_id from sold access");
        }
        if (qr_site_id == f_qr_site_id) {
            QDateTime timestamp = sold.value("timestamp").toDateTime();
            if (!timestamp.isNull()) {
                already_used = true;
                qInfo() << QString(" The ticket was used on - %1").arg(timestamp.toString());
            }
        }
    }

    if (!already_used) {
        foreach (auto site_id, site_ids) {
            if (site_id.toInt() == qr_site_id) {
                enable_access = true;
                break;
            }
        }
    }
EXIT:

    return enable_access;
}

bool QRServer::IsMultipleAccessTypeEnabled(const QList<QMap<QString, QVariant>> &sold_access,
                                           int &sale_id, int& aj_site_id, QString &code,
                                           const int &qr_site_id) {
    bool enable_access = false;
    bool already_used = false;
    QString codename;
    bool ok;

    foreach (auto sold, sold_access) {
        int f_qr_site_id = sold.value("qr_site_id").toInt(&ok);
        if (!ok) {
            GOTO_EXIT(EXIT, qDebug, "Can't get qr_site_id from sold access");;
        }
        if (qr_site_id == f_qr_site_id) {
            QDateTime timestamp = sold.value("timestamp").toDateTime();
            if (timestamp.isNull()) {
                enable_access = true;
            } else {
                //Found but already used
                already_used = true;
                GOTO_EXIT(EXIT, qInfo, QString(" The ticket was used on - %1").arg(timestamp.toString()));
            }
            break;
        }
    }

    if (!already_used && !enable_access) {

        QList<QMap<QString, QVariant>> dead_tickets;
        DBClient::Instance().GetDeadTickets(dead_tickets, "*",
                                            QString("sale_id = %1 and aj_site_id = %2"
                                                    " ALLOW FILTERING").arg(sale_id).
                                            arg(aj_site_id));
        int live_ctr = 0;

        if (dead_tickets.count() > 0) {
            live_ctr = dead_tickets[0].value("live_ctr").toInt(&ok);
            if (!ok) {
                GOTO_EXIT(EXIT, qDebug, "Can't get live_ctr from dead ticket");
            }
        }

        if (live_ctr > 0) {
            live_ctr--;
            const QList<QMap<QString, QVariant>> dead_tickets_update({
                                                                         {
                                                                             {"aj_site_id", aj_site_id},
                                                                             {"sale_id",    sale_id},
                                                                             {"code",       code},
                                                                             {"live_ctr",   live_ctr},
                                                                         }
                                                                     });

            ok = DBClient::Instance().InserRowsInDeadTickets(dead_tickets_update);
            if (!ok) {
                //TBD: Wrong code event generation.
                GOTO_EXIT(EXIT, qDebug, " can't update dead tickets entry");
            }
            enable_access = true;
        }
    }

EXIT:
    return enable_access;

}

void QRServer::SendVerificationResultViaMqtt(const QJsonObject &object, int &qr_site_id,
                                             int& door_id, bool enable_access) {
    //TBD: Define in configuration
    QString topic_simple = QString("site%1door%2/msg").arg(qr_site_id).arg(door_id);
    QString topic = QString("site%1door%2/full_msg").arg(qr_site_id).arg(door_id);

    QJsonDocument full_topic_json(object);
    MqttManager::Instance().publish(topic_simple, enable_access ? "true" : "false", 1, false);
    MqttManager::Instance().publish(topic, full_topic_json.toJson(), 1, false);

    qDebug() << "Json: " << QString::fromUtf8(full_topic_json.toJson().toStdString().data(),
                                              full_topic_json.toJson().toStdString().size());
}

bool QRServer::UpdateSoldAccesses(const QList<QMap<QString , QVariant>> &soldData) {
    return DBClient::Instance().InsertRowsInSoldAccessTable(soldData);
}

void QRServer::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QString qr(clientSocket->readAll());
    QString code;
    QStringList qr_list;
    QList<QMap<QString, QVariant>> code_access;
    QList<QMap<QString, QVariant>> fisc_unit;
    QList<QMap<QString, QVariant>> ajr_sales;
    QList<QMap<QString, QVariant>> sold_access;

    QString codename;
    bool enable_access = false;
    bool fiscal_check;
    bool ok;
    bool fail_over_flag = false;
    int aj_site_id = -1;
    int qr_site_id = -1;
    int door_id = -1;
    int sale_id = -1;
    int qty = 0;
    int access_type;
    QRServer::qr_t qr_bar;
    //TBD
    int LIFETIME = 10;


    qInfo() << "Check QR: " << qr;

    if (!ParseQrCode(qr, qr_site_id, door_id, codename) ) {
        GOTO_EXIT(EXIT, qDebug, codename);
    }

    fiscal_check = CheckFiscalUnits(qr, fisc_unit, qr_bar, codename);
    if (!fiscal_check) {
        GOTO_EXIT(EXIT, qDebug, codename);
    }

    if (DBClient::Instance().connState() == DBClient::CONNECTED) {

        /* TBD. Check are the current state is failover Level 1 */
        if (qr_bar == QR) {
            if (!DBClient::Instance().IsNodeIsAcive(fisc_unit[0].
                                                    value("fiscal_mem").
                                                    toString())) {
                //Fail over Level 1 - site where Ajure
                fail_over_flag = true;
            }
        }

        if (!GetAjrSales(ajr_sales, qr, codename)) {
            GOTO_EXIT(FAILOVER, qDebug, codename);
        }

        foreach (auto ajr_sale, ajr_sales) {

            code = ajr_sale.value("code").toString();
            qDebug() << "Ajure sale: " << ajr_sale;

            aj_site_id = ajr_sale.value("aj_site_id").toInt(&ok);
            if (!ok) {
                GOTO_EXIT(FAILOVER, qDebug, "Can't get site_id from ajur sale");
            }
            sale_id = ajr_sale.value("sale_id").toInt(&ok);
            if (!ok) {
                GOTO_EXIT(FAILOVER, qDebug, " Can't find get sale_id from ajur sale");
            }

            if (!GetSoldAccesForSale(sold_access, sale_id, aj_site_id, code)) {
                GOTO_EXIT(FAILOVER, qDebug, " Can't get Sold Access");
            }

            if(!DBClient::Instance().GetCodeAccessInfo(code_access, "*",
                                                       QString("code = '%1'").
                                                       arg(code))) {
                GOTO_EXIT(FAILOVER, qDebug, " Can't get Code Access");
            }

            if (code_access.count() <= 0) {
                GOTO_EXIT(FAILOVER, qDebug, "Can't find get code access");
            }
            qDebug() << "Code: " << code_access;
            /** access_type
                Single or Multiple museum access.
                If is 1 - access to one of sites deined in site_ids.
                If is 2 - access to all sites defined in site_ids.
            */
            access_type  = code_access[0].value("access_type").toInt(&ok);
            if (!ok) {
                GOTO_EXIT(FAILOVER, qDebug, " Can't get access_type from code access");
            }

            codename = code_access[0].value("codename", "Wrong Code Name").toString();

            if (access_type == SINGLE) {
                enable_access = IsSingleAccessTypeEnabled(sold_access,
                                                          code_access[0].value("site_ids").toList(),
                        qr_site_id);

            } else if (access_type == MULTIPLE) {
                enable_access = IsMultipleAccessTypeEnabled(sold_access, sale_id,
                                                            aj_site_id, code,
                                                            qr_site_id);
            }

            if (enable_access) {
                qty = ajr_sale.value("qty").toInt(&ok);
                if (!ok) {
                    GOTO_EXIT(FAILOVER, qDebug, "Can't get qty from ajur sale");
                }
                /* Ticket founded and verified in active nodes -
                    * clear failover flag */
                fail_over_flag = false;
                break;
            }
        }
FAILOVER:
        if (!enable_access) {
            /* Enable access on fail over Level 1 */
            if (fail_over_flag) {
                qCritical() << "Enable acces on fail over 1 state";
                //TBD ???
                qty = 100;
                enable_access = true;
            }
        }

        if (enable_access) {

            QDateTime cur_timestamp(QDateTime::currentDateTimeUtc());
            QList<QMap<QString , QVariant>> soldData({
                                                         {
                                                             {"aj_site_id",     aj_site_id},
                                                             {"qr_site_id",     qr_site_id},
                                                             {"sale_id",        sale_id},
                                                             {"code",           code},
                                                             {"door_id",        door_id},
                                                             {"qty",            qty},
                                                             {"used_cnt",       0},
                                                             {"fail_over_flag", fail_over_flag},
                                                             {"timestamp",      cur_timestamp},
                                                         }
                                                     });

            if (!UpdateSoldAccesses(soldData)) {
                GOTO_EXIT(EXIT, qDebug, "Can't update access");
            }
        }

    } else {
        // Failover logic level 2
        qInfo() << "QrSrv: No DB access activate failover level 2";
        enable_access = true;
    }

EXIT:
    if (enable_access)
    {
        qInfo() << "Open the door";
    }
    else {
        qInfo() << "Disable Access";
    }

    QJsonObject object =  {
        {"aj_site_id", qr_site_id},
        {"qr_site_id", qr_site_id},
        {"sale_id",    sale_id},
        {"code",       code},
        {"door_id",    door_id},
        {"qty",        qty},
        {"set_door",   enable_access ? "true" : "false"},
        {"codename",   codename},
    };

    SendVerificationResultViaMqtt(object, qr_site_id, door_id, enable_access);
    //clientSocket->write("Server says Hello");
}
