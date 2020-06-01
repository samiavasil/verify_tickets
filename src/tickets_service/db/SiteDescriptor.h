#ifndef SITEDESCRIPTOR_H
#define SITEDESCRIPTOR_H
#include "CassTable.h"

class SiteDescriptor : public CassTable
{
public:
    typedef enum {
        SITE_ID,
        NAME,
        INFO,
        DOORS,
        NODE_STATUS,
    } Column_t;

    SiteDescriptor(QString tableName = "site_descriptor", QString keySpace = QString());
    bool PrepareSiteDescriptorTable();

};

#endif // SITEDESCRIPTOR_H
