#ifndef SOLDACCESS_H
#define SOLDACCESS_H
#include "CassTable.h"

typedef struct{
    int mu_id  ;
    int sale_id;
} SoldAccess_t;

class SoldAccess : public CassTable
{
public:
    static SoldAccess& Instance();
    bool InserRowInSoldAccessTable(SoldAccess_t &data);
protected:
    SoldAccess(QString keySpace, QString tableName);
};

#endif // SOLDACCESS_H
