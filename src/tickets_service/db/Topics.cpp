#include "Topics.h"
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

static const QMap<Topics::Column_t, QPair<QString, QVariant::Type>> colType(
{
            {Topics::ID,     {"id",     QVariant::Int} },
            {Topics::ERROR, {"error", QVariant::String} },
            {Topics::SWITCH_ON, {"switch_on", QVariant::Int} },
        });

Topics &Topics::Instance()
{
    static Topics Topics("test_keyspace_xx", "Topics");
    return Topics;
}

Topics::Topics(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(fisc_str)")
{

}

bool Topics::InserRowInTopics(QMap<Topics::Column_t , QVariant> &row)
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

bool Topics::PrepareTopicsTable() {

    const QList<QMap<Topics::Column_t , QVariant>> dataList =  {
        {
            { Topics::ID           , "50179218"},
            { Topics::ERROR       , "Музей 1"},
            { Topics::SWITCH_ON       , 1},
        },
        {
            { Topics::ID           , "50179219"},
            { Topics::ERROR       , "Музей 2"},
            { Topics::SWITCH_ON       , 2},
        },
        {
            { Topics::ID          , "50179220"},
            { Topics::ERROR      , "Музей 3"},
            { Topics::SWITCH_ON      , 3},
        }
    };

    foreach (auto data, dataList) {
        ASSERT_ERROR("Insert row: ",InserRowInTopics(data));
    }
    return true;
}
