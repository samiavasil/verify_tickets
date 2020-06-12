#ifndef CASSTABLE_H
#define CASSTABLE_H

#include<QMap>
#include<QPair>
#include<QVariant>
#include<QString>

typedef struct CassSession_ CassSession;
typedef struct CassCluster_ CassCluster;

class CassTable
{
public:
    explicit CassTable( QString tableName,
                       const QList<QPair<QString, QVariant::Type>> &columns,
                       const QString& primKeys,
                       QString keySpace);

    virtual ~CassTable();

    QString keySpace() const;
    void setKeySpace(const QString &keySpace);

    QString tableName() const;
    void setTableName(const QString &tableName);

    virtual bool CreateTable(CassSession *session);
    virtual bool DropTable(CassSession *session);
    virtual bool CreateKeySpace(CassSession *session);
    virtual bool DropKeySpace(CassSession *session);
    virtual bool InsertRow(CassSession *session, const QMap<QString, QString> &data);
    virtual bool UpdateRow();
    virtual bool SelectFromTable(CassSession *session, QList<QMap<QString, QVariant>> &result,
                                 const QString &filter="*", const QString &where=QString());
    virtual bool InsertRowsInTable(CassSession *session, const QList<QMap<QString, QVariant> > &rows);

protected:
    QMap<QString, QVariant::Type> m_columns;
    QString m_tableName;
    QString m_keySpace;
    QString m_primKeys;
};

#endif // CASSTABLE_H
