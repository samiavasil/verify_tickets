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
    static CodeAccessInfo& Instance();
    bool InserRowInCodeAccessTable(CodeAccess_t &data);

    bool PrepareCodeAccessTable();
protected:
    CodeAccessInfo(QString keySpace, QString tableName);
};

#endif // CODEACCESSINFO_H
