#ifndef DEADTICKETS_H
#define DEADTICKETS_H
#include "CassTable.h"

class DeadTickets: public CassTable
{
public:
    typedef enum {
        AJ_SITE_ID,
        SALE_ID,
        LIVE_CTR
    } Column_t;

    static DeadTickets &Instance();
    bool InserRowInDeadTickets(QMap<DeadTickets::Column_t, QVariant> &row);
protected:
    DeadTickets(QString keySpace, QString tableName);

};

#endif // DEADTICKETS_H
