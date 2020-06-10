#include "QRServer.h"
#include "db/DBClient.h"
#include "MqttManager.h"
#include <QJsonDocument>
#include <qjsonobject.h>
#include "Configurator.h"

#define GOTO_EXIT(STR) do {\
    codename = (STR);\
    qInfo() << "Door access denied: " << (STR);\
    goto EXIT;\
    } while(0)\

QRServer::QRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}

QRServer::~QRServer()
{

}

bool QRServer::ParseQrCode(QString& qr, int &qr_site_id, int &door_id, QString& codename) {

    QStringList split_site_door;
    bool ok;
    bool ret = false;

    split_site_door = qr.split(":");

    if (split_site_door.count() >= 3 ) {
        qr_site_id = split_site_door[0].toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT("QR Code bad format");
        }

        qr.remove(0, split_site_door[0].length() + 1);
        /*Get door ID*/
        door_id = split_site_door[1].toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT("QR Code bad format");
        }
        qr.remove(0, split_site_door[1].count() + 1);
        ret = true;
    } else {
        qCritical() << "Not correct QR format (qr_site_id) should be:";
        qCritical() << "  qr_site_id:door_id:QR";
        //TBD: MQTT Wrong code event generation.
        GOTO_EXIT("QR Code bad format");
    }

EXIT:
    return ret;
}

bool QRServer::CheckFiscalUnits(QString &qr, QString &codename)
{
    QStringList qr_list;
    QList<QMap<QString, QVariant>> fisc_unit;
    bool ret = false;

    qr_list = qr.split("*");
    if (qr_list.count() > 0) {
        if (qr_list.count() > 1) {
            if(!DBClient::Instance().GetFiscalMem(fisc_unit,
                                                  "fiscal_mem",
                                                  QString("fiscal_mem='%1'  ALLOW FILTERING").
                                                  arg(qr_list[0]))) {
                GOTO_EXIT("Can't get fiscal mem");
            }
            //Check fiscal units
            if (fisc_unit.count() > 0) {
                qDebug() << "Продадено от фискален апарат" << fisc_unit[0].value("fiscal_mem");
                ret = true;
            }
        } else {
            /*TBD: logic for Barcodes here*/
            ret = true;
        }

    } else {
        qCritical() << "Not correct QR format (door_id) should be:";
        qCritical() << "  qr_site_id:door_id*QR";
        //TBD: MQTT Wrong code event generation.
        GOTO_EXIT("QR Code bad format");
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
            qCritical() << "Can't find this QR code";
            ret = false;
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT("Not valid QR code");
        }

        if (!ajr_sales[0].value("transfered").toBool()) {
            qCritical() << "Ajure sale isn't transfered. Try to transfer";
            if(!DBClient::Instance().TransferSoldAccess(ajr_sales)) {
                qCritical() << "Failure: Can't transfered Ajure sale.";
                ret = false;
                GOTO_EXIT("Not valid QR code. Please try again.");
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
                                         const   QVariantList  &code_access, const int &qr_site_id) {

    bool enable_access = false;
    bool already_used = false;

    foreach (auto sold, sold_access) {
        QDateTime timestamp = sold.value("timestamp").toDateTime();
        if (!timestamp.isNull()) {
            already_used = true;
            qDebug() << QString(" This ticket was used on - %1").arg(timestamp.toString());
        }
    }

    if (!already_used) {
        foreach (auto site_id, code_access) {
            if (site_id.toInt() == qr_site_id) {
                enable_access = true;
                break;
            }
        }
    }
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
            qCritical() << "Can't get qr_site_id";
            //TBD: Wrong code event generation.
            GOTO_EXIT(" sold access error");;
        }
        if (qr_site_id == f_qr_site_id) {
            QDateTime timestamp = sold.value("timestamp").toDateTime();
            if (timestamp.isNull()) {
                enable_access = true;
            } else {
                //Found but already used
                qDebug() << "The ticket was used on: " << timestamp;
                already_used = true;
                GOTO_EXIT(QString(" The ticket was used on - %1").arg(timestamp.toString()));
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
                qCritical() << "Can't get live_ctr";
                //TBD: Wrong code event generation.
                GOTO_EXIT(" dead tickets error");
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
                qCritical() << "Can't insert row in DeadTickets";
                //TBD: Wrong code event generation.
                GOTO_EXIT(" can't update dead tickets");
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
    bool ok;

    int aj_site_id;
    int qr_site_id = -1;
    int door_id = -1;
    int sale_id;
    int qty = 0;
    int access_type;
    //TBD
    int LIFETIME = 10;


    qDebug() << "QR: " << qr;

    if (!ParseQrCode(qr, qr_site_id, door_id, codename) ) {
        GOTO_EXIT(codename);
    }

    if (!CheckFiscalUnits(qr, codename)) {
        GOTO_EXIT(codename);
    }

    if (!GetAjrSales(ajr_sales, qr, codename)) {
        GOTO_EXIT(codename);
    }

    code = ajr_sales[0].value("code").toString();
    qDebug() << "Ajure sale: " << ajr_sales[0];

    aj_site_id = ajr_sales[0].value("aj_site_id").toInt(&ok);
    if (!ok) {
        qCritical() << "Can't get site_id";
        //TBD: Wrong code event generation.
        GOTO_EXIT(" ajur sale error");
    }
    sale_id = ajr_sales[0].value("sale_id").toInt(&ok);
    if (!ok) {
        qCritical() << "Can't find get sale_id";
        //TBD: Wrong code event generation.
        GOTO_EXIT(" ajur sale error");
    }

    if (!GetSoldAccesForSale(sold_access, sale_id, aj_site_id, code)) {
        GOTO_EXIT(" Can't get Sold Access");
    }

    DBClient::Instance().GetCodeAccessInfo(code_access, "*",
                                           QString("code = '%1'").
                                           arg(code));
    if (code_access.count() <= 0) {
        qCritical() << "Can't find get code access";
        //TBD: Wrong code event generation.
        GOTO_EXIT(" code acces error");
    }

    qDebug() << "Code: " << code_access;

    /** access_type
        Single or Multiple museum access.
        If is 1 - access to one of sites deined in site_ids.
        If is 2 - access to all sites defined in site_ids.
    */
    access_type  = code_access[0].value("access_type").toInt(&ok);
    if (!ok) {
        qCritical() << "Can't get access_type";
        //TBD: Wrong code event generation.
        GOTO_EXIT(" ajur sale error");
    }
    codename = code_access[0].value("codename", "Wrong Code Name").toString();

    if (access_type == SINGLE) {
        enable_access = IsSingleAccessTypeEnabled(sold_access,
                                                  code_access[0].value("site_ids").toList(),
                qr_site_id);

    } else if (access_type == MULTIPLE) {
        enable_access = IsMultipleAccessTypeEnabled(sold_access, sale_id, aj_site_id, code, qr_site_id);
    }

    if (enable_access) {

        QDateTime cur_timestamp(QDateTime::currentDateTimeUtc());

        //TBD add 1 full used_cnt
        qty = ajr_sales[0].value("qty").toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qty";
            //TBD: Wrong code event generation.
            GOTO_EXIT(" ajur sale error");
        }
        QList<QMap<QString , QVariant>> soldData({
                                                     {
                                                         {"aj_site_id",     aj_site_id},
                                                         {"qr_site_id",     qr_site_id},
                                                         {"sale_id",        sale_id},
                                                         {"code",           code},
                                                         {"door_id",        door_id},
                                                         {"qty",            qty},
                                                         {"used_cnt",       0},
                                                         {"fail_over_flag", false},
                                                         {"timestamp",      cur_timestamp},
                                                     }
                                                 });

        if (!UpdateSoldAccesses(soldData)) {
            qCritical() << "Can't insert row in SoldAccess";
            //TBD: Wrong code event generation.
            GOTO_EXIT(" update sold access");
        }

        qInfo() << "Open the door";

    } else {
        qInfo() << "Disable Access";
    }
EXIT:
    QJsonObject object =  {
        {"door_id",  door_id},
        {"site_id",  qr_site_id},
        {"code",     code},
        {"qty",      qty},
        {"set_door", enable_access ? "true" : "false"},
        {"codename", codename},
    };

    SendVerificationResultViaMqtt(object, qr_site_id, door_id, enable_access);
    //clientSocket->write("Server says Hello");
}
