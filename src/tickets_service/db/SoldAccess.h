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
        AJ_SITE_ID,
        SALE_ID,
        QR_SITE_ID,
        DOOR_ID,
        FAIL_OVER_FLAG,
        QTY,
        TIMESTAMP,
        USED_CNT,
    } Column_t;
    static SoldAccess& Instance();
    bool InserRowInSoldAccessTable(QMap<SoldAccess::Column_t, QVariant> &row);
protected:
    SoldAccess(QString tableName = "sold_Access", QString keySpace = QString());
};

#endif // SOLDACCESS_H
