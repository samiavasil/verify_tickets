#ifndef CASSTABLE_H
#define CASSTABLE_H

#include<QMap>
#include<QPair>
#include<QVariant>
#include<QString>
#include <cassandra.h>

class CassTable
{
public:
    explicit CassTable(QString keySpace, QString tableName,
                       const QList<QPair<QString, QVariant::Type>> &columns,
                       const QString& primKeys);
    virtual ~CassTable();

    QString keySpace() const;
    void setKeySpace(const QString &keySpace);

    QString tableName() const;
    void setTableName(const QString &tableName);

    virtual bool CreateTable();
    virtual bool DropTable();
    virtual bool CreateKeySpace();
    virtual bool DropKeySpace();
    virtual bool InsertRow(const QMap<QString, QString> &data);
    virtual bool UpdateRow();
    virtual bool SelectFromTable(QList<QMap<QString, QVariant>> &result,
                                 const QString &filter="*", const QString &where=QString());

protected:
    const QList<QPair<QString, QVariant::Type>> m_columns;
    QString m_tableName;
    QString m_keySpace;
    QString m_primKeys;
};

#endif // CASSTABLE_H
