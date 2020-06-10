#include "DeadTickets.h"
#include "MapQVarCass.h"

#define MU_ID_STR    "aj_site_id"
#define SALE_ID_STR  "sale_id"
#define LIVE_CTR_STR "live_ctr"

static const QList<QPair<QString, QVariant::Type>> colType(
        {
            {"aj_site_id", QVariant::Int},
            {"sale_id",    QVariant::Int},
            {"code",       QVariant::String},
            {"live_ctr",   QVariant::Int},
        });

DeadTickets::DeadTickets(QString tableName, QString keySpace):
    CassTable( tableName, colType, "(aj_site_id, sale_id, code)", keySpace)
{

}
