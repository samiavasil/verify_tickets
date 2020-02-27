#include "SiteDescriptor.h"
static const QMap<QString, QVariant::Type> columns({
                                                       {"mu_id", QVariant::Int},
                                                       {"name",  QVariant::String},
                                                       {"info",  QVariant::String},
                                                       {"access_type", QVariant::String},
                                                       {"obj_type", QVariant::String},
                                                       {"doors", QVariant::List},
                                                   });



SiteDescriptor::SiteDescriptor(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "(mu_id, name)")
{

}
