#include "SoldAccess.h"

#define ID             "id"
#define MUSEUM_ID      "mu_id"
#define SALE_ID        "sale_id"
#define SITE_ID        "site_id"
#define DOOR_ID        "door_id"
#define USED_CNT       "used_cnt"
#define LIFETIME       "lifetime"
#define FAIL_OVER_FLAG "fail_over_flag"
#define TIMESTAMP      "timestamp"





static const QMap<QString, QVariant::Type> columns({   {ID, QVariant::Uuid},
                                                       {MUSEUM_ID, QVariant::Int},
                                                       {SALE_ID, QVariant::Int},
                                                       {SITE_ID, QVariant::Int},
                                                       {DOOR_ID, QVariant::Int},
                                                       {USED_CNT, QVariant::Int},
                                                       {LIFETIME, QVariant::Int},
                                                       {FAIL_OVER_FLAG, QVariant::Bool},
                                                       {TIMESTAMP, QVariant::DateTime},
                                                   });


SoldAccess &SoldAccess::Instance()
{
    static SoldAccess sAccess("test_keyspace_xx", "sold_Access");
    return sAccess;
}

SoldAccess::SoldAccess(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "(id)")
{

}

bool SoldAccess::InserRowInSoldAccessTable(SoldAccess_t &data) {
    QMap<QString, QString> row;

    row.insert(ID, "uuid()");
    row.insert(MUSEUM_ID, QString("%1").arg(data.mu_id).toUtf8().constData());
    row.insert(SALE_ID, QString("%1").arg(data.sale_id).toUtf8().constData());
    /*=======================*/
    row.insert(DOOR_ID, QString("%1").arg(-1).toUtf8().constData());
    row.insert(LIFETIME, "0");
    row.insert(FAIL_OVER_FLAG, "false");
    row.insert(TIMESTAMP, "0");

    return InsertRow(row);
}
