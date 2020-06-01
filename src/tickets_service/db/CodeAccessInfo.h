#ifndef CODEACCESSINFO_H
#define CODEACCESSINFO_H
#include "CassTable.h"

class CodeAccessInfo: public CassTable
{
public:
    typedef enum {
        SINGLE   = 1,
        MULTIPLE = 2
    } AccessType_t;

    CodeAccessInfo(QString tableName = "code_Access_info", QString keySpace = QString());
    bool PrepareCodeAccessTable();
};

#endif // CODEACCESSINFO_H
