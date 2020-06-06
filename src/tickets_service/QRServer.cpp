#include "QRServer.h"
#include "db/DBClient.h"
#include "MqttManager.h"
#include <QJsonDocument>
#include <qjsonobject.h>
#include "Configurator.h"

#define GOTO_EXIT(STR) do {\
  codename = (STR);\
  qInfo() << "Door access denied.";\
  goto EXIT;\
} while(0)\

QRServer::QRServer(QObject *parent, const ServerConfigurator& config):
    TCPServer(parent, config)
{

}

QRServer::~QRServer()
{

}

void QRServer ::Receive()
{
    QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
    QString qr(clientSocket->readAll());
    bool enable_access = false;
    bool ok;
    bool already_used = false;
    int qr_site_id = -1;
    int door_id = -1;
    QString code;
    QStringList qr_list;
    QList<QMap<QString, QVariant>> code_access;
    QStringList split_site_door;
    QList<QMap<QString, QVariant>> fisc_unit;
    QList<QMap<QString, QVariant>> ajr_sale;
    QList<QMap<QString, QVariant>> sold_access;
    QString codename;
    int aj_site_id;
    int sale_id;
    int qty = 0;
    int access_type;
    //TBD
    int LIFETIME = 10;


    qDebug() << "QR: " << qr;
    split_site_door = qr.split(":");

    if (split_site_door.count() >= 3 ) {
        qr_site_id = split_site_door[0].toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT("QR Code bad format");
        }

        /* TBD ??? Check that QR site_id is the same as in configuration file
        if (qr_site_id != Configurator::Instance().site_id()) {
            qDebug() << "qr_site_id is not the same as site_id";
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT(STR);
        }*/

        qr.remove(0, split_site_door[0].length() + 1);
        /*Get door ID*/
        door_id = split_site_door[1].toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            GOTO_EXIT("QR Code bad format");
        }
        qr.remove(0, split_site_door[1].count() + 1);

    } else {
        qCritical() << "Not correct QR format (qr_site_id) should be:";
        qCritical() << "  qr_site_id:door_id:QR";
        //TBD: MQTT Wrong code event generation.
        GOTO_EXIT("QR Code bad format");
    }

    qr_list = qr.split("*");
    if(qr_list.count() > 0) {
        if (qr_list.count() > 1) {
            DBClient::Instance().GetFiscalMem(fisc_unit, "fiscal_mem",
                                              QString("fiscal_mem='%1'  ALLOW FILTERING").
                                              arg(qr_list[0]));
            //Check fiscal units
            if (fisc_unit.count() > 0) {
                qDebug() << "Продадено от фискален апарат" << fisc_unit[0].value("fiscal_mem");
            }
        }
    } else {
        qCritical() << "Not correct QR format (door_id) should be:";
        qCritical() << "  qr_site_id:door_id*QR";
        //TBD: MQTT Wrong code event generation.
        GOTO_EXIT("QR Code bad format");
    }

    DBClient::Instance().GetAjrSales(ajr_sale, "*",
                                     QString("qr='%1' ALLOW FILTERING").
                                     arg(qr));
    if (ajr_sale.count() <= 0) {
        qCritical() << "Can't find this QR code";
        //TBD: MQTT Wrong code event generation.
        GOTO_EXIT("Not valid QR code");
    }

    if (!ajr_sale[0].value("transfered").toBool()) {
        qCritical() << "Ajure sale isn't transfered. Try to transfer";
        if(!DBClient::Instance().TransferSoldAccess(ajr_sale)) {
            qCritical() << "Failure: Can't transfered Ajure sale.";
            GOTO_EXIT("Not valid QR code. Please try again.");
        }
        qDebug() << "Transfered";
    }

    code = ajr_sale[0].value("code").toString();
    qDebug() << "Ajure sale: " << ajr_sale[0];
    DBClient::Instance().GetCodeAccessInfo(code_access, "*",
                                           QString("code = '%1'").
                                           arg(code));
    if (code_access.count() <= 0) {
        qCritical() << "Can't find get code access";
        //TBD: Wrong code event generation.
        GOTO_EXIT("Acces denied: code acces error");
    }

    qDebug() << "Code: " << code_access;

    /** access_type
        Single or Multiple museum access.
        If is 1 - access to one of sites deined in site_ids.
        If is 2 - access to all sites defined in site_ids.
    */
    access_type  = code_access[0].value("access_type", -1).toInt();
    codename = code_access[0].value("codename", "Wrong Code Name").toString();
    aj_site_id = ajr_sale[0].value("aj_site_id").toInt(&ok);
    if (!ok) {
        qCritical() << "Can't get site_id";
        //TBD: Wrong code event generation.
        GOTO_EXIT("Acces denied: ajur sale error");
    }

    sale_id = ajr_sale[0].value("sale_id").toInt(&ok);
    if (!ok) {
        qCritical() << "Can't find get sale_id";
        //TBD: Wrong code event generation.
        GOTO_EXIT("Acces denied: ajur sale error");
    }


    DBClient::Instance().GetSoldAccess(sold_access, "*",
                                       QString("sale_id = %1 and aj_site_id = %2"
                                               " ALLOW FILTERING").
                                       arg(sale_id).
                                       arg(aj_site_id));

    if (access_type == SINGLE) {
        foreach (auto sold, sold_access) {
            QDateTime timestamp = sold.value("timestamp").toDateTime();
            if (!timestamp.isNull()) {
                already_used = true;
                qDebug() << "The ticket was used on: " << timestamp;
                GOTO_EXIT(QString("Acces denied: This ticket was used on - %1").arg(timestamp.toString()));
            }
        }

        if (!already_used) {
            foreach (auto site_id, code_access[0].value("site_ids").toList()) {
                if (site_id.toInt() == qr_site_id) {
                    enable_access = true;
                    break;
                }
            }
        }
    } else if (access_type == MULTIPLE) {

        foreach (auto sold, sold_access) {
            int f_qr_site_id = sold.value("qr_site_id").toInt(&ok);
            if (!ok) {
                qCritical() << "Can't get qr_site_id";
                //TBD: Wrong code event generation.
                GOTO_EXIT("Acces denied: sold access error");;
            }
            if (qr_site_id == f_qr_site_id) {
                QDateTime timestamp = sold.value("timestamp").toDateTime();
                if (timestamp.isNull()) {
                    enable_access = true;
                } else {
                    //Found but already used
                    qDebug() << "The ticket was used on: " << timestamp;
                    already_used = true;
                    GOTO_EXIT(QString("Acces denied: The ticket was used on - %1").arg(timestamp.toString()));
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
                    GOTO_EXIT("Acces denied: dead tickets error");
                }
            }

            if (live_ctr > 0) {
                enable_access = true;
                live_ctr--;
                const QList<QMap<QString, QVariant>> dead_tickets_update({
                                {
                                    {"aj_site_id", aj_site_id},
                                    {"sale_id",    sale_id},
                                    {"live_ctr",   live_ctr},
                                }
                                });

                ok = DBClient::Instance().InserRowsInDeadTickets(dead_tickets_update);
                if (!ok) {
                    qCritical() << "Can't insert row in DeadTickets";
                    //TBD: Wrong code event generation.
                    GOTO_EXIT("Acces denied: can't update dead tickets");
                }
            }
        }
    }

    if (enable_access) {

        QDateTime cur_timestamp(QDateTime::currentDateTimeUtc()); //TBD Add real timestamp

        //TBD add 1 full used_cnt
        qty = ajr_sale[0].value("qty").toInt(&ok);
        if (!ok) {
            qCritical() << "Can't get qty";
            //TBD: Wrong code event generation.
            GOTO_EXIT("Acces denied: ajur sale error");
        }
        const QList<QMap<QString , QVariant>> soldData({
                      {
                        {"aj_site_id",     aj_site_id},
                        {"qr_site_id",     qr_site_id},
                        {"sale_id",        sale_id},
                        {"door_id",        door_id},
                        {"qty",            qty},
                        {"used_cnt",       0},
                        {"fail_over_flag", false},
                        {"timestamp",      cur_timestamp},
                      }
                    });

        ok = DBClient::Instance().InsertRowsInSoldAccessTable(soldData);
        if (!ok) {
            qCritical() << "Can't insert row in SoldAccess";
            //TBD: Wrong code event generation.
            GOTO_EXIT("Acces denied: update sold access");
        }
        //Notify to open
        qty = ajr_sale[0].value("qty").toInt();
        qInfo() << "Open the door";

    }
EXIT:
    QJsonObject object =  {
        {"door_id",  door_id},
        {"site_id",  qr_site_id},
        {"qty",      qty},
        {"set_door", enable_access ? "true" : "false"},
        {"codename", codename},
    };

    //TBD: Define in configuration
    QString topic_simple = QString("site%1door%2/msg").arg(qr_site_id).arg(door_id);
    QString topic = QString("site%1door%2/full_msg").arg(qr_site_id).arg(door_id);

    QJsonDocument full_topic_json(object);

    MqttManager::Instance().publish(topic_simple, enable_access ? "true" : "false", 1, false);

    MqttManager::Instance().publish(topic, full_topic_json.toJson(), 1, false);

    qDebug() << "topic: "  << topic
             << ", Site: " << qr_site_id
             << ", Open the door: " << door_id
             << ", qty: " << qty;

    qDebug() << "Json: " << full_topic_json.toJson();


    //clientSocket->write("Server says Hello");
}
