#ifndef SITEDESCRIPTOR_H
#define SITEDESCRIPTOR_H
#include "CassTable.h"

class SiteDescriptor : public CassTable
{
public:
    SiteDescriptor(QString keySpace, QString tableName);
};

#endif // SITEDESCRIPTOR_H
