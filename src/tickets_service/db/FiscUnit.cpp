#include "FiscUnit.h"
#include "MapQVarCass.h"
#include <QDebug>

#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qDebug() << "Assert error:" << (text) << __func__ << " Line" << __LINE__; \
    return false; \
    } \
    qDebug() << (text)<< (ok); \
    } \
    while (0)

static const QList<QPair<QString, QVariant::Type>> colType({
            {"id",         QVariant::Int},
            {"fiscal_mem", QVariant::String},
            {"site_id",    QVariant::Int},
        });

FiscUnit::FiscUnit(QString tableName, QString keySpace):
    CassTable(tableName, colType, "(id)", keySpace)
{

}

bool FiscUnit::PrepareFiscUnitTable() {

    const QList<QMap<QString , QVariant>> dataList =  {
    };

    ASSERT_ERROR("Insert row: ", InsertRowsInTable(dataList));
    return true;
}
