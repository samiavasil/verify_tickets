#include "AJRSale.h"
#include <QDebug>
#include "db/MapQVarCass.h"


#define MUSEUM_ID_STR    "mu_id"
#define SALE_ID_STR      "sale_id"
#define QR_STR           "qr"
#define CODE_STR         "code"
#define CODENAME_STR     "codename"
#define QTY_STR          "qty"
#define TIMESTAMP_IN_STR "timestam_in"
#define TRANSFERED_STR   "transfered"

static const QMap<AJRSale::Column_t, QPair<QString, QVariant::Type>> colType(
                                                    {
                                                       {AJRSale::MU_ID,       {MUSEUM_ID_STR,    QVariant::Int}     },
                                                       {AJRSale::SALE_ID,     {SALE_ID_STR,      QVariant::Int}     },
                                                       {AJRSale::QR,          {QR_STR,           QVariant::String}  },
                                                       {AJRSale::CODE,        {CODE_STR,         QVariant::String}  },
                                                       {AJRSale::CODENAME,    {CODENAME_STR,     QVariant::String}  },
                                                       {AJRSale::QTY,         {QTY_STR,          QVariant::Int}     },
                                                       {AJRSale::TIMESTAMP_IN,{TIMESTAMP_IN_STR, QVariant::DateTime} },
                                                       {AJRSale::TRANSFERED,  {TRANSFERED_STR,   QVariant::Bool}    },
                                                   });

AJRSale &AJRSale::Instance()
{
    static AJRSale ajrSale("test_keyspace_xx", "Ajur_Sale");
    return ajrSale;
}

AJRSale::AJRSale(QString keySpace, QString tableName):
    CassTable(keySpace, tableName, colType.values(), "mu_id, sale_id")
{

}

bool AJRSale::InserRowInSalesTable(QMap<AJRSale::Column_t , QVariant> &row)
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