#include "DeadTickets.h"
#include "MapQVarCass.h"

#define MU_ID_STR    "mu_id"
#define SALE_ID_STR  "sale_id"
#define LIVE_CTR_STR "live_ctr"

static const QMap<DeadTickets::Column_t, QPair<QString, QVariant::Type>> colType(
{
            {DeadTickets::MU_ID,       {"mu_id",   QVariant::Int}     },
            {DeadTickets::SALE_ID,     {"sale_id", QVariant::Int} },
            {DeadTickets::LIVE_CTR,    {"live_ctr",QVariant::Int}  },
        });


DeadTickets &DeadTickets::Instance()
{
    static DeadTickets DeadTickets("test_keyspace_xx", "deadTickets");
    return DeadTickets;
}


DeadTickets::DeadTickets(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "(mu_id, sale_id)")
{

}

bool DeadTickets::InserRowInDeadTickets(QMap<DeadTickets::Column_t , QVariant> &row)
{
    QMap<QString, QString> row_str;
    auto col = row.constBegin();
    while (col != row.constEnd()) {
        QString str;
        MapQVarCass::convertQVariantToStrout(col.value(), colType.value(col.key()).second, str);
        row_str.insert(colType.value(col.key()).first, str);
        col++;
    }
    return InsertRow(row_str);
}
