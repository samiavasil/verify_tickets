#ifndef TOPICS_H
#define TOPICS_H

#include "CassTable.h"

class Topics: public CassTable
{
public:

    typedef enum {
        ID,
        ERROR,
        SWITCH_ON
    } Column_t;

    static Topics &Instance();
    bool InserRowInTopics(QMap<Topics::Column_t, QVariant> &row);

public slots:
    bool PrepareTopicsTable();
protected:
    Topics(QString keySpace, QString tableName);

};
#endif // TOPICS_H
