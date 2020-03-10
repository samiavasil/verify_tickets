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


static const QMap<CodeAccessInfo::Column_t, QPair<QString, QVariant::Type>> colType(
                                                    {
                                                       {CodeAccessInfo::CODE,      {"code",      QVariant::String}  },
                                                       {CodeAccessInfo::CODENAME,  {"codename",  QVariant::String}  },
                                                       {CodeAccessInfo::INFO,      {"info",      QVariant::String}  },
                                                       {CodeAccessInfo::DEADLEVEL, {"deadlevel", QVariant::Int}     },
                                                       {CodeAccessInfo::SITE_IDS,     {"site_ids",    QVariant::List}    },
                                                   });

CodeAccessInfo::CodeAccessInfo(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "code")
{

}

CodeAccessInfo &CodeAccessInfo::Instance()
{
    static CodeAccessInfo codeAccessInfo("test_keyspace_xx", "code_Access_info");
    return codeAccessInfo;
}

bool CodeAccessInfo::InserRowInCodeAccessTable(QMap<CodeAccessInfo::Column_t , QVariant> &row)
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

bool CodeAccessInfo::PrepareCodeAccessTable() {

    const QList<QMap<CodeAccessInfo::Column_t , QVariant>> dataList =  {
        {
            {CodeAccessInfo::CODE,      "1016"},
            {CodeAccessInfo::CODENAME,  "Единичен Възр."},
            {CodeAccessInfo::INFO,      "Цена 6лв"},
            {CodeAccessInfo::DEADLEVEL, 0},
            {CodeAccessInfo::SITE_IDS,     QVariant::fromValue<QList<int>>({1,2,3})}
        },
        {
            {CodeAccessInfo::CODE,      "1003"},
            {CodeAccessInfo::CODENAME,  "Единичен Дете."},
            {CodeAccessInfo::INFO,      "Цена 3лв"},
            {CodeAccessInfo::DEADLEVEL, 0},
            {CodeAccessInfo::SITE_IDS,     QVariant::fromValue<QList<int>>({17})}
        },
        {
            {CodeAccessInfo::CODE,     "1118"},
            {CodeAccessInfo::CODENAME, "Арх. + Етн. Възр."},
            {CodeAccessInfo::INFO,     "Цена 8лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,2})}
        },
        {
            {CodeAccessInfo::CODE,     "1104"},
            {CodeAccessInfo::CODENAME, "Арх. + Етн. Дете"},
            {CodeAccessInfo::INFO,     "Цена 4лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,2})}
        },
        {
            {CodeAccessInfo::CODE,     "11110"},
            {CodeAccessInfo::CODENAME, "Арх. + Стефан Възр."},
            {CodeAccessInfo::INFO,     "Цена 10лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {CodeAccessInfo::CODE,     "11105"},
            {CodeAccessInfo::CODENAME, "Арх. + Стефан Дете"},
            {CodeAccessInfo::INFO,     "Цена 5лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {CodeAccessInfo::CODE,     "11115"},
            {CodeAccessInfo::CODENAME, "Арх. + Стефан + 2 Възр."},
            {CodeAccessInfo::INFO,     "Цена 15лв"},
            {CodeAccessInfo::DEADLEVEL,2},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {CodeAccessInfo::CODE,     "11108"},
            {CodeAccessInfo::CODENAME, "Арх. + Стефан + 2  Дете"},
            {CodeAccessInfo::INFO,     "Цена 8лв"},
            {CodeAccessInfo::DEADLEVEL,2},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({1,3})}
        },
        {
            {CodeAccessInfo::CODE,     "111018"},
            {CodeAccessInfo::CODENAME, "Всички 6 църкви  Възр."},
            {CodeAccessInfo::INFO,     "Цена 18лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({4,5,6,7,8,9})}
        },
        {
            {CodeAccessInfo::CODE,     "11009"},
            {CodeAccessInfo::CODENAME, "Всички 6 църкви  Дете"},
            {CodeAccessInfo::INFO,     "Цена 9лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({4,5,6,7,8,9})}
        },
        {
            {CodeAccessInfo::CODE,     "11125"},
            {CodeAccessInfo::CODENAME, "Всички 8 музея  Възр."},
            {CodeAccessInfo::INFO,     "Цена 25лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({10,11,16,17})}
        },
        {
            {CodeAccessInfo::CODE,     "11012"},
            {CodeAccessInfo::CODENAME, "Всички 8 музея  Дете"},
            {CodeAccessInfo::INFO,     "Цена 12лв"},
            {CodeAccessInfo::DEADLEVEL,0},
            {CodeAccessInfo::SITE_IDS,    QVariant::fromValue<QList<int>>({16,17})}
        },
    };
        qDebug() << dataList[0].value(CodeAccessInfo::SITE_IDS).type();
    foreach (auto data, dataList) {
       ASSERT_ERROR("Insert row: ",InserRowInCodeAccessTable(data));
    }
    return true;
}
