#include "CodeAccessInfo.h"
#include <QDebug>

#define CODE         "code"
#define CODENAME     "codename"
#define INFO         "info"
#define DEADLEVEL "deadlevel"
#define MUIDS   "mu_ids"


#define ASSERT_ERROR(text, check)  do { \
    bool ok = (check); \
    if(!ok) { \
    qDebug() << "Assert error:" << (text) << __func__ << " Line" << __LINE__; \
    return false; \
    } \
    qDebug() << (text)<< (ok); \
    } \
    while (0)

static const QMap<QString, QVariant::Type> columns({
                                                     {CODE        , QVariant::String},
                                                     {CODENAME    , QVariant::String},
                                                     {INFO        , QVariant::String},
                                                     {DEADLEVEL   , QVariant::Int},
                                                     {MUIDS       , QVariant::List},
                                                   });

CodeAccessInfo::CodeAccessInfo(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "code, codename, deadlevel")
{

}

CodeAccessInfo &CodeAccessInfo::Instance()
{
    static CodeAccessInfo codeAccessInfo("test_keyspace_xx", "code_Access_info");
    return codeAccessInfo;
}

bool CodeAccessInfo::InserRowInCodeAccessTable(CodeAccess_t &data) {
    QMap<QString, QString> row;

    row.insert(CODE     , QString("'%1'").arg(data.code).toUtf8().constData());
    row.insert(CODENAME , QString("'%1'").arg(data.codename).toUtf8().constData());
    row.insert(INFO     , QString("'%1'").arg(data.info).toUtf8().constData());
    row.insert(DEADLEVEL, QString("%1").arg(data.deadLevel).toUtf8().constData());
    QString muids;
    foreach(auto mu_id, data.muIDs) {
        if (!muids.isEmpty()) {
            muids.append(", ");
        }
        muids.append(QString("%1").arg(mu_id));
    }
    muids.prepend("{");
    muids.append("}");
    row.insert(MUIDS   , QString("%1").arg(muids).toUtf8().constData());

    return InsertRow(row);
}

bool CodeAccessInfo::PrepareCodeAccessTable() {
    const QList<CodeAccess_t> dataList =  {
        {"1016", "Единичен Възр.", "Цена 6лв", 0, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17} },
        {"1003", "Единичен Дете.", "Цена 3лв", 0, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17} },
        {"1118", "Арх. + Етн. Възр.", "Цена 8лв", 0, {1,2} },
        {"1104", "Арх. + Етн. Дете", "Цена 4лв", 0, {1,2} },
        {"11110", "Арх. + Стефан Възр.", "Цена 10лв", 0, {1,3} },
        {"11105", "Арх. + Стефан Дете", "Цена 5лв", 0, {1,3} },
        {"11115", "Арх. + Стефан + 2 Възр.", "Цена 15лв", 2, {1,3} },
        {"11108", "Арх. + Стефан + 2  Дете", "Цена 8лв", 2, {1,3} },
        {"111018", "Всички 6 църкви  Възр.", "Цена 18лв", 2, {4,5,6,7,8,9} },
        {"11009", "Всички 6 църкви  Дете", "Цена 9лв", 2, {4,5,6,7,8,9} },
        {"11125", "Всички 6 църкви  Възр.", "Цена 25лв", 2, {10,11,12,13,14,15,16,17} },
        {"11012", "Всички 6 църкви  Дете", "Цена 12лв", 2, {10,11,12,13,14,15,16,17} },
    };


    foreach (auto data, dataList) {
       ASSERT_ERROR("Insert row: ",InserRowInCodeAccessTable(data));
    }
    return true;
}
