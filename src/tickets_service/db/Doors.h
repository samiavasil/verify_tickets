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

    static Doors &Instance();
    bool InserRowInDoors(QMap<Doors::Column_t, QVariant> &row);

public slots:
    bool PrepareDoorsTable();
protected:
    Doors(QString tableName = "Doors", QString keySpace = QString());

};
#endif // DOORS_H
