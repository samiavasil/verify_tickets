#ifndef CODEACCESSINFO_H
#define CODEACCESSINFO_H
#include "CassTable.h"

typedef struct {
    QString code;
    QString codename;
    QString info;
    int deadLevel;
    QList<int> muIDs;
} CodeAccess_t;


class CodeAccessInfo: public CassTable
{
public:
    typedef enum {
        CODE,
        CODENAME,
        INFO,
        DEADLEVEL,
        MUIDS,
    } Column_t;

    static CodeAccessInfo& Instance();
    bool PrepareCodeAccessTable();
    bool InserRowInCodeAccessTable(QMap<CodeAccessInfo::Column_t, QVariant> &row);

protected:
    CodeAccessInfo(QString keySpace, QString tableName);
};

#endif // CODEACCESSINFO_H
