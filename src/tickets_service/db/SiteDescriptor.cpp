#include "MapQVarCass.h"
#include "SiteDescriptor.h"
#include <QDebug>

static const QList<QPair<QString, QVariant::Type>> colType({
            {"site_id",     QVariant::Int},
            {"name",        QVariant::String},
            {"info",        QVariant::String},
            {"doors",       QVariant::List},
            {"node_status", QVariant::Bool},
        });

SiteDescriptor::SiteDescriptor(QString tableName, QString keySpace):
    CassTable( tableName, colType, "(site_id)", keySpace)
{

}

bool SiteDescriptor::PrepareSiteDescriptorTable(CassSession *session) {

    const QList<QMap<QString , QVariant>> dataList =  {
        {
            {"site_id"     , 1},
            {"name"        , "Музей 1"},
            {"info"        , "Питай на касата :Р"},
            {"doors"        , QVariant::fromValue<QList<int>>({1,2})},
            {"node_status", true },
        },
        {
            { "site_id"       , 2},
            { "name"        , "Музей 2"},
            { "info"        , "Това да не ти е бюро справки ? :Р"},
            {"doors"        , QVariant::fromValue<QList<int>>({3,4,5})},
            {"node_status", true },
        },
        {
            { "site_id"       , 3},
            { "name"        , "Музей 3"},
            { "info"        , "Абе я си взимай билет и не питай."},
            {"doors"        , QVariant::fromValue<QList<int>>({6,7})},
            {"node_status", true },
        }
    };

    qDebug() << dataList[0].value("doors").type();
    ASSERT_ERROR("Insert row: ",InsertRowsInTable(session, dataList));
    return true;
}
