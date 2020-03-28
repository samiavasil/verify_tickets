#include "Doors.h"
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

static const QMap<Doors::Column_t, QPair<QString, QVariant::Type>> colType({
            {Doors::ID,      {"id",     QVariant::Int} },
            {Doors::SITE_ID, {"name_address", QVariant::Int} },
            {Doors::NAME,    {"name", QVariant::QVariant::String} },
            {Doors::TOPIC,   {"topic", QVariant::Int} },
        });

Doors &Doors::Instance()
{
    static Doors Doors("test_keyspace_xx", "Doors");
    return Doors;
}

Doors::Doors(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(fisc_str)")
{

}

bool Doors::InserRowInDoors(QMap<Doors::Column_t , QVariant> &row)
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

bool Doors::PrepareDoorsTable() {

    const QList<QMap<Doors::Column_t , QVariant>> dataList =  {
        {
            { Doors::ID       , 1},
            { Doors::SITE_ID  , 1},
            { Doors::NAME     , "Музей 1. Предна Вратня"},
        },
        {
            { Doors::ID       , 2},
            { Doors::SITE_ID  , 1},
            { Doors::NAME     , "Музей 1. Задна Вратня"},
        },
        {
            { Doors::ID       , 3},
            { Doors::SITE_ID  , 2},
            { Doors::NAME     , "Музей 2. Единствена Вратня"},
        }
    };

    foreach (auto data, dataList) {
        ASSERT_ERROR("Insert row: ",InserRowInDoors(data));
    }
    return true;
}
