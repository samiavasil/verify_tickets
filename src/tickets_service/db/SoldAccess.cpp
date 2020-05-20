#include "MapQVarCass.h"
#include "SoldAccess.h"

static const QMap<SoldAccess::Column_t, QPair<QString, QVariant::Type>> colType(
{
            {SoldAccess::SALE_ID,        {"sale_id", QVariant::Int}  },
            {SoldAccess::AJ_SITE_ID,     {"aj_site_id", QVariant::Int}},
            {SoldAccess::QR_SITE_ID,     {"qr_site_id", QVariant::Int}  },
            {SoldAccess::DOOR_ID,        {"door_id", QVariant::Int}  },
            {SoldAccess::FAIL_OVER_FLAG, {"fail_over_flag", QVariant::Bool} },
            {SoldAccess::QTY,             {"qty", QVariant::Int}  },
            {SoldAccess::TIMESTAMP,      {"timestamp", QVariant::DateTime} },
            {SoldAccess::USED_CNT,       {"used_cnt", QVariant::Int} },
        }
        );

SoldAccess &SoldAccess::Instance()
{
    static SoldAccess sAccess;
    return sAccess;
}

SoldAccess::SoldAccess(QString tableName, QString keySpace):
    CassTable(tableName, colType.values(), "(aj_site_id, sale_id, qr_site_id)", keySpace)
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
