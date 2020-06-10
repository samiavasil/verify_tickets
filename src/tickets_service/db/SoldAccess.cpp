#include "MapQVarCass.h"
#include "SoldAccess.h"

static const QList<QPair<QString, QVariant::Type>> colType({
            {"sale_id", QVariant::Int},
            {"aj_site_id", QVariant::Int},
            {"qr_site_id", QVariant::Int},
            {"code",       QVariant::String},
            {"door_id", QVariant::Int},
            {"fail_over_flag", QVariant::Bool},
            {"qty", QVariant::Int},
            {"timestamp", QVariant::DateTime},
            {"used_cnt", QVariant::Int},
        });

SoldAccess::SoldAccess(QString tableName, QString keySpace):
    CassTable(tableName, colType, "(aj_site_id, sale_id, code, qr_site_id)", keySpace)
{

}
