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
            {FiscUnit::FISC_STR,     {"fisc_str",     QVariant::String} },
            {FiscUnit::NAME_ADDRESS, {"name_address", QVariant::String} },
            {FiscUnit::CASS_NODE_ID, {"cass_node_id", QVariant::Int} },
        });

FiscUnit &FiscUnit::Instance()
{
    static FiscUnit FiscUnit("test_keyspace_xx", "fiscUnit");
    return FiscUnit;
}

FiscUnit::FiscUnit(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(fisc_str)")
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
        {
            { FiscUnit::FISC_STR           , "50179218"},
            { FiscUnit::NAME_ADDRESS       , "Музей 1"},
            { FiscUnit::CASS_NODE_ID       , 1},
        },
        {
            { FiscUnit::FISC_STR           , "50179219"},
            { FiscUnit::NAME_ADDRESS       , "Музей 2"},
            { FiscUnit::CASS_NODE_ID       , 2},
        },
        {
            { FiscUnit::FISC_STR          , "50179220"},
            { FiscUnit::NAME_ADDRESS      , "Музей 3"},
            { FiscUnit::CASS_NODE_ID      , 3},
        }
    };

    foreach (auto data, dataList) {
        ASSERT_ERROR("Insert row: ",InserRowInFiscUnit(data));
    }
    return true;
}
