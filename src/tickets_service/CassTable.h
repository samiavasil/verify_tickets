#ifndef CASSTABLE_H
#define CASSTABLE_H

#include<QMap>
#include<QVariant>
#include<QString>
#include "cassandra.h"

class CassTable
{
public:
    explicit CassTable(QString keySpace, QString tableName, QMap<QString,  QVariant::Type> &columns);
    virtual ~CassTable();
    virtual bool CreateTable(CassSession* session);
    virtual bool DropTable(CassSession* session);
    virtual bool CreateKeySpace(CassSession* session);
    virtual bool DropKeySpace(CassSession* session);
    QString keySpace() const;
    void setKeySpace(const QString &keySpace);

    QString tableName() const;
    void setTableName(const QString &tableName);
    virtual bool InsertRow(CassSession *session, const QMap<QString, QString> &data);
    virtual bool UpdateRow();
protected:
    const QMap<QString, QVariant::Type> m_columns;
    QString m_tableName;
    QString m_keySpace;
};

#endif // CASSTABLE_H
