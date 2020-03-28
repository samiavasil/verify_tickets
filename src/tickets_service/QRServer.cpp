#include "QRServer.h"
#include "db/AJRSale.h"
#include "db/CodeAccessInfo.h"
#include "db/DeadTickets.h"
#include "db/FiscUnit.h"
#include "db/SiteDescriptor.h"
#include "db/SoldAccess.h"
#include "Comar.h"
#include <QJsonDocument>
#include <qjsonobject.h>

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
   /*TBD: Read from QR*/
    int qr_site_id = 5;
    int door_id = 3;
    //TBD
    int LIFETIME = 10;
 qDebug() << "QR: " << qr;
    QStringList split_site_door = qr.split(":");
    if (split_site_door.count() >= 3 ) {
        qr_site_id = split_site_door[0].toInt(&ok);
        if (!ok) {
            qDebug() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            return;
        }
        qr.remove(0, split_site_door[0].length() + 1);

        door_id = split_site_door[1].toInt(&ok);
        if (!ok) {
            qDebug() << "Can't get qr_site_id";
            //TBD: MQTT Wrong code event generation.
            return;
        }
        qr.remove(0, split_site_door[1].count() + 1);

    } else {
        qDebug() << "Not correct QR format (qr_site_id) should be:";
        qDebug() << "  qr_site_id:door_id:QR";
        //TBD: MQTT Wrong code event generation.
        return;
    }

    QStringList list = qr.split("*");
    qDebug() << qr << list;

    if(list.count() > 0) {

        if (list.count() > 1) {
            //QR = 50179218*181*2020-02-28*20:07:00*25
            QList<QMap<QString, QVariant>> fisc_unit;
            FiscUnit::Instance().SelectFromTable(fisc_unit, "fisc_str",
                                                 QString("fisc_str='%1'").
                                                 arg(list[0]));
            //Check fiscal units
            if (fisc_unit.count() > 0) {
                qDebug() << "Продадено от фискален апарат" << fisc_unit[0].value("fisc_str");
            }
        }
    } else {
        qDebug() << "Not correct QR format (door_id) should be:";
        qDebug() << "  qr_site_id:door_id*QR";
        //TBD: MQTT Wrong code event generation.
        return;
    }

    QList<QMap<QString, QVariant>> ajr_sale;
    AJRSale::Instance().SelectFromTable(ajr_sale, "*",
                                        QString("qr='%1' ALLOW FILTERING").arg(qr));
    if (ajr_sale.count() <= 0) {
        qDebug() << "Can't find this QR code";
        //TBD: MQTT Wrong code event generation.
        return;
    }

    QString code = ajr_sale[0].value("code").toString();
    qDebug() << "Ajure sale: " << ajr_sale[0];

    QList<QMap<QString, QVariant>> code_access;
    CodeAccessInfo::Instance().SelectFromTable(code_access, "*",
                                               QString("code = '%1'").
                                               arg(code));
    if (code_access.count() <= 0) {
        qDebug() << "Can't find get code access";
        //TBD: Wrong code event generation.
        return;
    }

    qDebug() << "Code: " << code_access;

    /** access_type
        Single or Multiple museum access.
        If is 0 - access to one of sites deined in site_ids.
        If is 1 - access to all sites defined in site_ids.
    */
    int access_type  = code_access[0].value("access_type", -1).toInt();
    QString codename = code_access[0].value("codename", "").toString();
    auto site_ids = code_access[0].value("site_ids").toList();

    int aj_site_id = ajr_sale[0].value("aj_site_id").toInt(&ok);
    if (!ok) {
        qDebug() << "Can't get site_id";
        //TBD: Wrong code event generation.
        return;
    }

    int sale_id = ajr_sale[0].value("sale_id").toInt(&ok);
    if (!ok) {
        qDebug() << "Can't find get sale_id";
        //TBD: Wrong code event generation.
        return;
    }

    QList<QMap<QString, QVariant>> sold_access;
    SoldAccess::Instance().SelectFromTable(sold_access, "*",
                                           QString("sale_id = %1 and aj_site_id = %2"
                                                   " ALLOW FILTERING").
                                           arg(sale_id).
                                           arg(aj_site_id));

    if (access_type == SINGLE) {
        foreach (auto sold, sold_access) {
            QDateTime timestamp = sold.value("timestamp").toDateTime();
            if (!timestamp.isNull()) {
                already_used = true;
                qDebug() << "This ticket was used on: " << timestamp;
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
    } else if (access_type == MULTIPLE) {

        foreach (auto sold, sold_access) {
            int f_qr_site_id = sold.value("qr_site_id").toInt(&ok);
            if (!ok) {
                qDebug() << "Can't get qr_site_id";
                //TBD: Wrong code event generation.
                return;
            }
            if (qr_site_id == f_qr_site_id) {
                QDateTime timestamp = sold.value("timestamp").toDateTime();
                if (timestamp.isNull()) {
                    enable_access = true;
                } else {
                    //Found but already used
                    qDebug() << "This ticket was used on: " << timestamp;
                    already_used = true;
                }
                break;
            }
        }

        if (!already_used && !enable_access) {

            QList<QMap<QString, QVariant>> dead_tickets;
            DeadTickets::Instance().SelectFromTable(dead_tickets, "*",
                                                    QString("sale_id = %1 and aj_site_id = %2"
                                                            " ALLOW FILTERING").arg(sale_id).
                                                    arg(aj_site_id)
                                                    );

            int live_ctr = 0;
            if (dead_tickets.count() > 0) {
                live_ctr = dead_tickets[0].value("live_ctr").toInt(&ok);
                if (!ok) {
                    qDebug() << "Can't get live_ctr";
                    //TBD: Wrong code event generation.
                    return;
                }
            }

            if (live_ctr > 0) {
                enable_access = true;
                live_ctr--;
                QMap<DeadTickets::Column_t , QVariant> dead_tickets_update(
                {
                                {DeadTickets::AJ_SITE_ID, aj_site_id},
                                {DeadTickets::SALE_ID,    sale_id},
                                {DeadTickets::LIVE_CTR,   live_ctr},
                            });

                ok = DeadTickets::Instance().InserRowInDeadTickets(dead_tickets_update);
                if (!ok) {
                    qDebug() << "Can't get insert row in SoldAccess";
                    //TBD: Wrong code event generation.
                    return;
                }
            }
        }
    }

    if (enable_access) {
        QDateTime cur_timestamp;
        cur_timestamp.setMSecsSinceEpoch(1213342); //TBD Add real timestamp
        //TBD add 1 full sold access
        QMap<SoldAccess::Column_t , QVariant> soldData(
        {
                        {SoldAccess::AJ_SITE_ID,     aj_site_id},
                        {SoldAccess::QR_SITE_ID,     qr_site_id},
                        {SoldAccess::SALE_ID,        sale_id},
                        {SoldAccess::DOOR_ID,        door_id},
                        {SoldAccess::USED_CNT,       0},
                        {SoldAccess::FAIL_OVER_FLAG, false},
                        {SoldAccess::TIMESTAMP,      cur_timestamp},
                    });

        ok = SoldAccess::Instance().InserRowInSoldAccessTable(soldData);
        if (!ok) {
            qDebug() << "Can't get insert row in SoldAccess";
            //TBD: Wrong code event generation.
            return;
        }
        //Notify to open
        int qty = ajr_sale[0].value("qty").toInt();
        QString topic_simple = QString("site%1door%2/msg").arg(qr_site_id).arg(door_id);
        QString topic = QString("site%1door%2/full_msg").arg(qr_site_id).arg(door_id);
        QJsonObject object =  {
            {"door_id",  door_id},
            {"site_id",  qr_site_id},
            {"qty",      qty},
            {"set_door", enable_access?"true" : "false"},
            {"codename", codename},
        };
        QJsonDocument json(object);

        Comar::Instance().publish(topic, enable_access?"true" : "false", 1, false);

        Comar::Instance().publish(topic, json.toJson(), 1, false);

        qDebug() << "topic: "  << topic
                 << ", Site: " << qr_site_id
                 << ", Open the door: " << door_id
                 << ", qty: " << qty;

        qDebug() << "Json: " << json.toJson();

        qDebug() << "Open the door";

    }


    //clientSocket->write("Server says Hello");
}
