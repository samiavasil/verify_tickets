#include "FiscUnit.h"
#include "MapQVarCass.h"
#include <QDebug>

static const QList<QPair<QString, QVariant::Type>> colType({
            {"id",         QVariant::Int},
            {"fiscal_mem", QVariant::String},
            {"site_id",    QVariant::Int},
        });

FiscUnit::FiscUnit(QString tableName, QString keySpace):
    CassTable(tableName, colType, "(id)", keySpace)
{

}

bool FiscUnit::PrepareFiscUnitTable(CassSession *session) {

    const QList<QMap<QString , QVariant>> dataList =  {
    };

    ASSERT_ERROR("Insert row: ", InsertRowsInTable(session, dataList));
    return true;
}
