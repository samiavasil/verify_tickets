#ifndef SOLDACCESS_H
#define SOLDACCESS_H
#include "CassTable.h"

class SoldAccess : public CassTable
{
public:
    SoldAccess(QString tableName = "sold_Access", QString keySpace = QString());
};

#endif // SOLDACCESS_H
