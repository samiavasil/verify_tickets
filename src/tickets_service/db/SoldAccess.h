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

    typedef enum {
/*        ID,*/
        MUSEUM_ID,
        SALE_ID,
        SITE_ID,
        DOOR_ID,
        USED_CNT,
        LIFETIME,
        FAIL_OVER_FLAG,
        TIMESTAMP
    } Column_t;
    static SoldAccess& Instance();
    bool InserRowInSoldAccessTable(QMap<SoldAccess::Column_t, QVariant> &row);
protected:
    SoldAccess(QString keySpace, QString tableName);
};

#endif // SOLDACCESS_H
