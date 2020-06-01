#include "AJRSale.h"
#include <QDebug>
#include "db/MapQVarCass.h"


#define MUSEUM_ID_STR    "aj_site_id"
#define SALE_ID_STR      "sale_id"
#define QR_STR           "qr"
#define CODE_STR         "code"
#define CODENAME_STR     "codename"
#define QTY_STR          "qty"
#define TIMESTAMP_IN_STR "timestamp_in"
#define TRANSFERED_STR   "transfered"

static const  QList<QPair<QString, QVariant::Type>> colType(
                                                    {
                                                       {MUSEUM_ID_STR,    QVariant::Int}     ,
                                                       {SALE_ID_STR,      QVariant::Int}     ,
                                                       {QR_STR,           QVariant::String}  ,
                                                       {CODE_STR,         QVariant::String}  ,
                                                       {CODENAME_STR,     QVariant::String}  ,
                                                       {QTY_STR,          QVariant::Int}     ,
                                                       {TIMESTAMP_IN_STR, QVariant::DateTime},
                                                       {TRANSFERED_STR,   QVariant::Bool}    ,
                                                   });


AJRSale::AJRSale(QString tableName, QString keySpace):
    CassTable( tableName, colType, "aj_site_id, sale_id", keySpace)
{

}
