#ifndef DOORS_H
#define DOORS_H

#include "CassTable.h"

class Doors: public CassTable
{
public:

    typedef enum {
        ID,
        SITE_ID,
        NAME,
        TOPIC
    } Column_t;

    static Doors &Instance();
    bool InserRowInDoors(QMap<Doors::Column_t, QVariant> &row);

public slots:
    bool PrepareDoorsTable();
protected:
    Doors(QString keySpace, QString tableName);

};
#endif // DOORS_H