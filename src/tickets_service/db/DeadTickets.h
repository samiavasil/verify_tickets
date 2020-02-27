#ifndef DEADTICKETS_H
#define DEADTICKETS_H
#include "CassTable.h"

class DeadTickets: public CassTable
{
public:
    DeadTickets(QString keySpace, QString tableName);
};

#endif // DEADTICKETS_H
