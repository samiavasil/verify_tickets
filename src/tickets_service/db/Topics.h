#ifndef TOPICS_H
#define TOPICS_H

#include "CassTable.h"

class Topics: public CassTable
{
public:

    static Topics &Instance();

public slots:
    bool PrepareTopicsTable(CassSession *session);
protected:
    Topics(QString tableName = "Topics", QString keySpace = QString());

};
#endif // TOPICS_H
