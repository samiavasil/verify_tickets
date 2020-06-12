#ifndef DBCLIENT_H
#define DBCLIENT_H
#include <QObject>
#include <QString>
#include "AJRSale.h"
#include "CodeAccessInfo.h"
#include "DeadTickets.h"
#include "Doors.h"
#include "FiscUnit.h"
#include "SiteDescriptor.h"
#include "SoldAccess.h"

class DBClient:public QObject
{
    Q_OBJECT
public:
    enum connectionState{
        /*Activated on start if all node are down */
        NOT_CONNECTED,
        /*Activated if at least one node is active*/
        CONNECTED,
        /*Activated on lost connection (previous state is CONNECTED)*/
        DISCONNECTED
    };

    static DBClient& Instance();
    virtual ~DBClient();
    bool connectSession();
    CassSession *session() const;
    connectionState connState() const;
    void setConnState(const connectionState &connState);
    bool TransferSoldAccess(QList<QMap<QString, QVariant> > &data);
    void DoConsistencyTransfer();
    bool GetFiscalMem(QList<QMap<QString, QVariant> > &fisc_unit, const QString &filter, const QString &where);
    bool GetAjrSales(QList<QMap<QString, QVariant> > &ajr_sale, const QString &filter, const QString &where);
    bool GetCodeAccessInfo(QList<QMap<QString, QVariant> > &code_access, const QString &filter, const QString &where);
    bool GetSoldAccess(QList<QMap<QString, QVariant> > &sold_access, const QString &filter, const QString &where);
    bool GetDeadTickets(QList<QMap<QString, QVariant> > &dead_tickets, const QString &filter, const QString &where);
    bool InserRowsInDeadTickets(const QList<QMap<QString, QVariant> > &dead_tickets_rows);
    bool InsertRowsInSoldAccessTable(const QList<QMap<QString, QVariant> > &sold_data_rows);
    bool InsertRowsInAjrSale(const QList<QMap<QString, QVariant> > &sold_data_rows);
    bool FindFiscalMem(QList<QMap<QString, QVariant>> &fisc_unit, QString &qr);
    bool InstallSessionConnector();
    bool LoadFiscalMems();
    bool IsNodeIsAcive(const QString &fisc_mem);
protected:
    DBClient();
    void PrepareTables();

protected:
    CassCluster* m_cluster;
    CassSession* m_session;
    connectionState m_connState;
    AJRSale m_AJRSale;
    CodeAccessInfo m_CodeAccessInfo;
    DeadTickets m_DeadTickets;
    Doors m_Doors;
    FiscUnit m_FiscUnit;
    SiteDescriptor m_SiteDescriptor;
    SoldAccess m_SoldAccess;
    QList<QMap<QString, QVariant>> m_fisc_unit;

protected slots:
    void timerDbConect();
};

#endif // DBCLIENT_H
