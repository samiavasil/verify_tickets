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

static const QMap<FiscUnit::Column_t, QPair<QString, QVariant::Type>> colType(
{
            {FiscUnit::ID,         {"id",         QVariant::Int} },
            {FiscUnit::FISCAL_MEM, {"fiscal_mem", QVariant::String} },
            {FiscUnit::SITE_ID,    {"site_id",    QVariant::Int} },
        });

FiscUnit &FiscUnit::Instance()
{
    static FiscUnit FiscUnit;
    return FiscUnit;
}

FiscUnit::FiscUnit(QString tableName, QString keySpace):
    CassTable(tableName, colType.values(), "(id)", keySpace)
{

}

bool FiscUnit::InserRowInFiscUnit(QMap<FiscUnit::Column_t , QVariant> &row)
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

bool FiscUnit::PrepareFiscUnitTable() {

    const QList<QMap<FiscUnit::Column_t , QVariant>> dataList =  {
    };

    foreach (auto data, dataList) {
        ASSERT_ERROR("Insert row: ",InserRowInFiscUnit(data));
    }
    return true;
}
