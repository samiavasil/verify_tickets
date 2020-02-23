#include "CassTable.h"

#include<QDebug>
#include "BuildQuerry.h"

CassTable::CassTable(QString keySpace, QString tableName, QMap<QString, QString> &columns):
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
    bool ret = false;


            BuildQuerry querry;

    querry.create().table(QString("%1.%2").arg(m_keySpace).arg(m_tableName)).
            lbrace();
    QMap<QString, QString>::const_iterator col = m_columns.constBegin();
    while (col != m_columns.constEnd()) {
        querry.keyType(col.key(), col.value());
        ++col;
    }
    querry.primaryKey("key1").rbrace().semicolon();

    qDebug() << querry.querry();


    return (CASS_OK == execute_query(session, querry.querry().toUtf8().constData()));

}

bool CassTable::InsertRow()
{
    bool ret = false;

    return ret;
}

bool CassTable::UpdateRow()
{
    bool ret = false;

    return ret;
}
