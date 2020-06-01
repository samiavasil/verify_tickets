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

    DeadTickets(QString tableName = "deadTickets", QString keySpace = QString());
};

#endif // DEADTICKETS_H
