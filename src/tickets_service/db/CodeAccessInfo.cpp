#include "CodeAccessInfo.h"
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
                                                       {"code",      QVariant::String},
                                                       {"codename",  QVariant::String},
                                                       {"info",      QVariant::String},
                                                       {"deadlevel", QVariant::Int}   ,
                                                       {"access_type", QVariant::Int} ,
                                                       {"site_ids",  QVariant::List}  ,
                                                   });

CodeAccessInfo::CodeAccessInfo( QString tableName, QString keySpace):
    CassTable( tableName, colType, "code", keySpace)
{

}

bool CodeAccessInfo::PrepareCodeAccessTable() {

    const QList<QMap<QString, QVariant>> dataList =  {
        {
            {"code",      "1016"},
            {"codename",  "Единичен Възр."},
            {"info",      "Цена 6лв"},
            {"deadlevel", 0},
            {"access_type", CodeAccessInfo::SINGLE},
            {"site_ids",     QVariant::fromValue<QList<int>>({1,2,3,4,5,6,7,8})}
        },
        {
            {"code",      "1003"},
            {"codename",  "Единичен Дете."},
            {"info",      "Цена 3лв"},
            {"deadlevel", 0},
            {"access_type", CodeAccessInfo::SINGLE},
            {"site_ids",     QVariant::fromValue<QList<int>>({1,3,4,6,8})}
        },
        {
            {"code",     "1118"},
            {"codename", "Арх. + Етн. Възр."},
            {"info",     "Цена 8лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,2})}
        },
        {
            {"code",     "1104"},
            {"codename", "Арх. + Етн. Дете"},
            {"info",     "Цена 4лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,2})}
        },
        {
            {"code",     "11110"},
            {"codename", "Арх. + Стефан Възр."},
            {"info",     "Цена 10лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {"code",     "11105"},
            {"codename", "Арх. + Стефан Дете"},
            {"info",     "Цена 5лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {"code",     "11115"},
            {"codename", "Арх. + Стефан + 2 Възр."},
            {"info",     "Цена 15лв"},
            {"deadlevel",2},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {"code",     "11108"},
            {"codename", "Арх. + Стефан + 2  Дете"},
            {"info",     "Цена 8лв"},
            {"deadlevel",2},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {"code",     "111018"},
            {"codename", "Всички 6 църкви  Възр."},
            {"info",     "Цена 18лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,4,5,6,7,8})}
        },
        {
            {"code",     "11009"},
            {"codename", "Всички 6 църкви  Дете"},
            {"info",     "Цена 9лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,4,5,6,7,8})}
        },
        {
            {"code",     "11125"},
            {"codename", "Всички 8 музея  Възр."},
            {"info",     "Цена 25лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,2,3,4,5,6,7,8})}
        },
        {
            {"code",     "11012"},
            {"codename", "Всички 8 музея  Дете"},
            {"info",     "Цена 12лв"},
            {"deadlevel",0},
            {"access_type", CodeAccessInfo::MULTIPLE},
            {"site_ids",    QVariant::fromValue<QList<int>>({1,2,3,4,5,6,7,8})}
        },
    };

    ASSERT_ERROR("Insert row: ",InsertRowsInTable(dataList));

    return true;
}
