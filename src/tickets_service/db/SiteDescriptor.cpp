#include "MapQVarCass.h"
#include "SiteDescriptor.h"
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

static const QMap<SiteDescriptor::Column_t, QPair<QString, QVariant::Type>> colType(
{
            {SiteDescriptor::MU_ID,       {"mu_id",       QVariant::Int} },
            {SiteDescriptor::NAME,        {"name",        QVariant::String} },
            {SiteDescriptor::INFO,        {"info",        QVariant::String} },
            {SiteDescriptor::ACCESS_TYPE, {"access_type", QVariant::String} },
            {SiteDescriptor::OBJ_TYPE,    {"obj_type",    QVariant::String} },
            {SiteDescriptor::DOORS,       {"doors",       QVariant::List} }
        }
        );

SiteDescriptor::SiteDescriptor(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(mu_id)")
{

}

SiteDescriptor &SiteDescriptor::Instance()
{
    static SiteDescriptor SiteDescriptor("test_keyspace_xx", "site_descriptor");
    return SiteDescriptor;
}

bool SiteDescriptor::InserRowInSiteDescriptorTable(QMap<SiteDescriptor::Column_t , QVariant> &row)
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

bool SiteDescriptor::PrepareSiteDescriptorTable() {

    const QList<QMap<SiteDescriptor::Column_t , QVariant>> dataList =  {
        {
            { SiteDescriptor::MU_ID       , 1},
            { SiteDescriptor::NAME        , "Музей 1"},
            { SiteDescriptor::INFO        , "Питай на касата :Р"},
            { SiteDescriptor::ACCESS_TYPE , 0},
            { SiteDescriptor::OBJ_TYPE    , 1},
            {SiteDescriptor::DOORS        , QVariant::fromValue<QList<int>>({1,2})}
        },
        {
            { SiteDescriptor::MU_ID       , 2},
            { SiteDescriptor::NAME        , "Музей 2"},
            { SiteDescriptor::INFO        , "Това да не ти е бюро справки ? :Р"},
            { SiteDescriptor::ACCESS_TYPE , 0},
            { SiteDescriptor::OBJ_TYPE    , 1},
            {SiteDescriptor::DOORS        , QVariant::fromValue<QList<int>>({3,4,5})}
        },
        {
            { SiteDescriptor::MU_ID       , 3},
            { SiteDescriptor::NAME        , "Музей 3"},
            { SiteDescriptor::INFO        , "Абе я си взимай билет и не питай."},
            { SiteDescriptor::ACCESS_TYPE , 0},
            { SiteDescriptor::OBJ_TYPE    , 2},
            {SiteDescriptor::DOORS        , QVariant::fromValue<QList<int>>({6,7})}
        }
    };

    qDebug() << dataList[0].value(SiteDescriptor::DOORS).type();
    foreach (auto data, dataList) {
        ASSERT_ERROR("Insert row: ",InserRowInSiteDescriptorTable(data));
    }
    return true;
}
