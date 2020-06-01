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

static const QList<QPair<QString, QVariant::Type>> colType(
{
            {"id",     QVariant::Int},
            {"error", QVariant::String},
            {"switch_on", QVariant::Int},
        });

Topics &Topics::Instance()
{
    static Topics Topics;
    return Topics;
}

Topics::Topics(QString tableName, QString keySpace):
    CassTable(tableName, colType, "(fiscal_mem)", keySpace)
{

}

bool Topics::PrepareTopicsTable() {

    const QList<QMap<QString, QVariant>> dataList =  {
        {
            { "id"        , "50179218"},
            { "error"     , "Музей 1"},
            { "switch_on" , 1},
        },
        {
            { "id"        , "50179219"},
            { "error"     , "Музей 2"},
            { "switch_on" , 2},
        },
        {
            { "id"        , "50179220"},
            { "error"     , "Музей 3"},
            { "switch_on" , 3},
        }
    };

    ASSERT_ERROR("Insert row: ",InsertRowsInTable(dataList));
    return true;
}
