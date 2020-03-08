#ifndef DEADTICKETS_H
#define DEADTICKETS_H
#include "CassTable.h"

class DeadTickets: public CassTable
{
public:
    typedef enum {
        MU_ID,
        SALE_ID,
        LIVE_CTR
    } Column_t;

    DeadTickets(QString keySpace, QString tableName);
    bool InserRowInDeadTickets(QMap<DeadTickets::Column_t, QVariant> &row);
};

#endif // DEADTICKETS_H
