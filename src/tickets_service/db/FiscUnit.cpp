#include "FiscUnit.h"

static const QMap<QString, QVariant::Type> columns({
                                                       {"fisc_str",     QVariant::String},
                                                       {"name_address", QVariant::String},
                                                       {"cass_node_id", QVariant::Int},
                                                   });


FiscUnit::FiscUnit(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "(fisc_str)")
{

}
