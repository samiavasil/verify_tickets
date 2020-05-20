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

    static SiteDescriptor& Instance();
    bool PrepareSiteDescriptorTable();
    bool InserRowInSiteDescriptorTable(QMap<SiteDescriptor::Column_t, QVariant> &row);

protected:
    SiteDescriptor(QString tableName = "site_descriptor", QString keySpace = QString());
};

#endif // SITEDESCRIPTOR_H
