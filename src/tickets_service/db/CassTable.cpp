#include "CassTable.h"
#include "BuildQuery.h"
#include "MapQVarCass.h"
#include "DBClient.h"
#include<QDebug>

#define CHECK_SESSION(x) do { \
    if (x == nullptr) { \
    return false; \
    } \
    } while(0)

CassTable::CassTable(QString keySpace, QString tableName,
                     const QList<QPair<QString, QVariant::Type> > &columns,
                     const QString& primKeys):
    m_columns(columns),
    m_tableName(tableName),
    m_keySpace(keySpace),
    m_primKeys(primKeys)
{

}

CassTable::~CassTable()
{

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

void print_error(CassFuture* future) {
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);
    fprintf(stderr, "Error: %.*s\n", int(message_length), message);
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

bool CassTable::CreateTable()
{
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();

    CHECK_SESSION(session);
    query.createTable(QString("%1.%2").arg(m_keySpace).arg(m_tableName)).
            lbrace();

   auto col = m_columns.constBegin();
    while (col != m_columns.constEnd()) {
        if( col->second == QVariant::List) {
            //TODO: Fix me for any list type
            //MapQVarCass::qvarTypeCassName(col.value())
            query.collectionSetType(col->first,"INT" );
        } else {
            query.keyType(col->first,MapQVarCass::qvarTypeCassName(col->second));
        }

        ++col;
    }
    query.primaryKey(m_primKeys).rbrace().semicolon();

    qDebug() << query.query();

    return (CASS_OK == execute_query(session, query.query().toUtf8().constData()));

}

bool CassTable::DropTable()
{
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();

    CHECK_SESSION(session);
    query.dropTable(QString("%1.%2").arg(m_keySpace).arg(m_tableName));

    qDebug() << query.query();

    return (CASS_OK == execute_query(session, query.query().toUtf8().constData()));
}

bool CassTable::CreateKeySpace()
{
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();

    CHECK_SESSION(session);
    query.createKeySpace(m_keySpace).replication(BuildQuery::SimpleStrategy, 2);

    qDebug() << query.query();

    return (CASS_OK == execute_query(session, query.query().toUtf8().constData()));
}

bool CassTable::DropKeySpace()
{
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();

    CHECK_SESSION(session);
    query.dropKeySpace(m_keySpace);

    qDebug() << query.query();

    return (CASS_OK == execute_query(session, query.query().toUtf8().constData()));
}

bool CassTable::InsertRow(const QMap<QString, QString> &data)
{
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();

    CHECK_SESSION(session);
    query.insertInto(QString("%1.%2").arg(m_keySpace).arg(m_tableName)).lbrace();
    /*
     *   INSERT INTO test_keyspace.site (id, name, info)
            VALUES (uuid(), 'KRUIKSWIJK','Steven')
            USING TTL 86400 AND TIMESTAMP 123456789;
          */

    QMap<QString, QString>::const_iterator col = data.constBegin();
    while (col != data.constEnd()) {
        query.addStr(col.key());
        ++col;
        if (col != data.constEnd()) {
            query.comma();
        } else {
            break;
        }
    }
    query.rbrace().values().lbrace();

    col = data.constBegin();
    while (col != data.constEnd()) {
        query.value(col.value());
        ++col;
        if (col != data.constEnd()) {
            query.comma();
        } else {
            break;
        }
    }
    query.rbrace().semicolon();
    qDebug() << query.query();
    return (CASS_OK == execute_query(session, query.query().toUtf8().constData()));
}

bool CassTable::UpdateRow()
{
    bool ret = false;

    return ret;
}

bool  CassTable::SelectFromTable(const QString &filter, QMap<QString, QVariant> &result) {

    CassError rc = CASS_OK;
    CassStatement* statement = nullptr;
    CassFuture* future = nullptr;
    BuildQuery query;
    CassSession *session = DBClient::Instance().session();
    const CassResult* cas_res = nullptr;
    bool ret = false;
    CHECK_SESSION(session);

    result.clear();
    query.select(filter).from(QString("%1.%2").arg(m_keySpace).arg(m_tableName));

    qDebug() << query.query();

    statement = cass_statement_new(query.query().toUtf8().constData(), 0);

    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    } else {
        cas_res = cass_future_get_result(future);
    }

    cass_future_free(future);
    cass_statement_free(statement);

    if (cas_res != nullptr) {

        CassIterator* rows = nullptr;
        const CassRow* row;
        const CassValue* col_val;
        rows = cass_iterator_from_result(cas_res);

        while (cass_iterator_next(rows)) {

            QVariant res;
            auto col = m_columns.constBegin();
            qDebug() << "===============================================";
            while (col != m_columns.constEnd()) {

                row = cass_iterator_get_row(rows);
                col_val = cass_row_get_column_by_name(row, col->first.toUtf8().
                                                      constData());
                MapQVarCass::convertCassToQVariant(col_val, col->second, res);
                qDebug() << col->first << ": " << res.toString();
                ++col;
            }
        }
        qDebug() << "===============================================";
        cass_result_free(cas_res);
        cass_iterator_free(rows);
        ret = true;
    }

    return ret;
}
