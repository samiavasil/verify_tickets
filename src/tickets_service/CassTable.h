#ifndef CASSTABLE_H
#define CASSTABLE_H

#include<QMap>
#include<QVariant>
#include<QString>
#include "cassandra.h"

class CassTable
{
public:
    explicit CassTable(QString keySpace, QString tableName, QMap<QString, QString> &columns);
    virtual ~CassTable();
    virtual bool CreateTable(CassSession* session);
    virtual bool CreateKeySpace(CassSession* session);
    QString keySpace() const;
    void setKeySpace(const QString &keySpace);

    QString tableName() const;
    void setTableName(const QString &tableName);

protected:

    virtual bool InsertRow();
    virtual bool UpdateRow();
protected:
    const QMap<QString, QString> m_columns;
    QString m_tableName;
    QString m_keySpace;
};

#endif // CASSTABLE_H
