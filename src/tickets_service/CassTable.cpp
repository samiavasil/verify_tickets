#include "CassTable.h"

#include<QDebug>
#include "BuildQuerry.h"

CassTable::CassTable(QString keySpace, QString tableName, QMap<QString, QVariant::Type> &columns):
    m_columns(columns),
    m_tableName(tableName),
    m_keySpace(keySpace)
{

}

CassTable::~CassTable()
{

}

void print_error(CassFuture* future) {
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);
    fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassError execute_query(CassSession* session, const char* query) {
    CassError rc = CASS_OK;
    CassStatement* statement = cass_statement_new(query, 0);

    CassFuture* future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    cass_statement_free(statement);
    cass_future_free(future);
    return rc;
}

bool CassTable::CreateTable(CassSession* session)
{
    BuildQuerry querry;

    querry.createTable(QString("%1.%2").arg(m_keySpace).arg(m_tableName)).
            lbrace();

    QMap<QString, QVariant::Type>::const_iterator col = m_columns.constBegin();
    while (col != m_columns.constEnd()) {
/*Todo:
Fixme - something smart to convert types is needed. This is a just temporary fix.
*/
        QVariant val = col.value();
        QString name =  QString( val.type() != QVariant::String ? val.typeName() : "text");
        querry.keyType(col.key(),name);
        ++col;
    }
    querry.primaryKey("key1").rbrace().semicolon();

    qDebug() << querry.querry();

    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));

}

bool CassTable::DropTable(CassSession *session)
{
    BuildQuerry querry;
    querry.dropTable(QString("%1.%2").arg(m_keySpace).arg(m_tableName));

    qDebug() << querry.querry();

    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));
}

bool CassTable::CreateKeySpace(CassSession *session)
{
    BuildQuerry querry;
    querry.createKeySpace(m_keySpace).replication(BuildQuerry::SimpleStrategy, 2);

    qDebug() << querry.querry();

    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));
}

bool CassTable::DropKeySpace(CassSession *session)
{
    BuildQuerry querry;
    querry.dropKeySpace(m_keySpace);

    qDebug() << querry.querry();

    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));
}

bool CassTable::InsertRow(CassSession *session, const QMap<QString, QString> &data)
{
    BuildQuerry querry;
    querry.insertInto(QString("%1.%2").arg(m_keySpace).arg(m_tableName)).lbrace();
    /*
     *   INSERT INTO test_keyspace.site (id, name, info)
            VALUES (uuid(), 'KRUIKSWIJK','Steven')
            USING TTL 86400 AND TIMESTAMP 123456789;
          */

    QMap<QString, QString>::const_iterator col = data.constBegin();
    while (col != data.constEnd()) {
        querry.key(col.key());
        ++col;
        if (col != data.constEnd()) {
            querry.comma();
        } else {
            break;
        }
    }
    querry.rbrace().values().lbrace();

    col = data.constBegin();
    while (col != data.constEnd()) {
        querry.value(col.value());
        ++col;
        if (col != data.constEnd()) {
            querry.comma();
        } else {
            break;
        }
    }
    querry.rbrace();
    qDebug() << querry.querry();
    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));
}

bool CassTable::UpdateRow()
{
    bool ret = false;

    return ret;
}

QString CassTable::tableName() const
{
    return m_tableName;
}

void CassTable::setTableName(const QString &tableName)
{
    m_tableName = tableName;
}

QString CassTable::keySpace() const
{
    return m_keySpace;
}

void CassTable::setKeySpace(const QString &keySpace)
{
    m_keySpace = keySpace;
}
