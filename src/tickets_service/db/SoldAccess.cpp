#include "MapQVarCass.h"
#include "SoldAccess.h"

static const QMap<SoldAccess::Column_t, QPair<QString, QVariant::Type>> colType(
{
/*            {SoldAccess::ID,             {"id", QVariant::Uuid}      },*/
            {SoldAccess::MUSEUM_ID,      {"mu_id", QVariant::Int}},
            {SoldAccess::SALE_ID,        {"sale_id", QVariant::Int}  },
            {SoldAccess::SITE_ID,        {"site_id", QVariant::Int}  },
            {SoldAccess::DOOR_ID,        {"door_id", QVariant::Int}  },
            {SoldAccess::USED_CNT,       {"used_cnt", QVariant::Int} },
            {SoldAccess::LIFETIME,       {"lifetime", QVariant::Int} },
            {SoldAccess::FAIL_OVER_FLAG, {"fail_over_flag", QVariant::Bool} },
            {SoldAccess::TIMESTAMP,      {"timestamp", QVariant::DateTime} }
        }
        );

SoldAccess &SoldAccess::Instance()
{
    static SoldAccess sAccess("test_keyspace_xx", "sold_Access");
    return sAccess;
}

SoldAccess::SoldAccess(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(mu_id, sale_id, site_id)")
{

}

bool SoldAccess::InserRowInSoldAccessTable(QMap<SoldAccess::Column_t , QVariant> &row)
{
    QMap<QString, QString> row_str;
    auto col = row.constBegin();
    while (col != row.constEnd()) {
        QString str;
        MapQVarCass::convertQVariantToStrout(col.value(), colType.value(col.key()).second, str);
        row_str.insert(colType.value(col.key()).first, str);
        col++;
    }
    return InsertRow(row_str);
}
