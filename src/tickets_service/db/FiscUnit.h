#ifndef FISCUNIT_H
#define FISCUNIT_H
#include "CassTable.h"

class FiscUnit: public CassTable
{
public:

    typedef enum {
        FISC_STR,
        NAME_ADDRESS,
        CASS_NODE_ID
    } Column_t;

    static FiscUnit &Instance();
    bool InserRowInFiscUnit(QMap<FiscUnit::Column_t, QVariant> &row);

public slots:
    bool PrepareFiscUnitTable();
protected:
    FiscUnit(QString tableName = "fiscUnit", QString keySpace = QString());

};

#endif // FISCUNIT_H
