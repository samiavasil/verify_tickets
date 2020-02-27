#include "DeadTickets.h"


static const QMap<QString, QVariant::Type> columns({
                                                       {"mu_id", QVariant::Int},
                                                       {"sale_id", QVariant::Int},
                                                       {"live_ctr", QVariant::Int},
                                                   });

DeadTickets::DeadTickets(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, columns, "(mu_id, sale_id)")
{

}
