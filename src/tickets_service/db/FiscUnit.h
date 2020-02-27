#ifndef FISCUNIT_H
#define FISCUNIT_H
#include "CassTable.h"

class FiscUnit: public CassTable
{
public:
    FiscUnit(QString keySpace, QString tableName);
};

#endif // FISCUNIT_H
