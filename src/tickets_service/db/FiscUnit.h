#ifndef FISCUNIT_H
#define FISCUNIT_H
#include "CassTable.h"

class FiscUnit: public CassTable
{
public:
   FiscUnit(QString tableName = "fiscUnit", QString keySpace = QString());
   bool PrepareFiscUnitTable();
};

#endif // FISCUNIT_H
