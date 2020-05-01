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
        ACCESS_TYPE,
        SITE_IDS,
    } Column_t;

    typedef enum {
        SINGLE   = 1,
        MULTIPLE = 2
    } AccessType_t;

    static CodeAccessInfo& Instance();
    bool PrepareCodeAccessTable();
    bool InserRowInCodeAccessTable(QMap<CodeAccessInfo::Column_t, QVariant> &row);

protected:
    CodeAccessInfo(QString tableName = "code_Access_info", QString keySpace = QString());
};

#endif // CODEACCESSINFO_H
