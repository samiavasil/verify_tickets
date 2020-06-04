#ifndef DOORS_H
#define DOORS_H

#include "CassTable.h"

class Doors: public CassTable
{
public:

    typedef enum {
        DOOR_ID,
        SITE_ID,
        NAME,
        INFO
    } Column_t;

    Doors(QString tableName = "Doors", QString keySpace = QString());
    bool PrepareDoorsTable(CassSession *session);
};
#endif // DOORS_H
