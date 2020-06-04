#include "Doors.h"
#include "MapQVarCass.h"
#include <QDebug>

static const QList<QPair<QString, QVariant::Type>> colType({
             {"door_id",     QVariant::Int},
             {"site_id", QVariant::Int},
             {"name", QVariant::QVariant::String},
             {"info", QVariant::String},
        });

Doors::Doors( QString tableName, QString keySpace):
    CassTable(tableName, colType, "(door_id, site_id)", keySpace)
{

}


bool Doors::PrepareDoorsTable(CassSession *session) {

    const QList<QMap<QString , QVariant>> dataList =  {
        {
            { "door_id"  , 1},
            { "site_id"  , 1},
            { "name"     , "Музей 1. Предна Вратня"},
        },
        {
            { "door_id"  , 2},
            { "site_id"  , 1},
            { "name"     , "Музей 1. Задна Вратня"},
        },
        {
            { "door_id"  , 3},
            { "site_id"  , 2},
            { "name"     , "Музей 2. Единствена Вратня"},
        }
    };

    ASSERT_ERROR("Insert row: ", InsertRowsInTable(session, dataList));
    return true;
}
