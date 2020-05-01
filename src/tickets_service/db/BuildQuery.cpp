#include "BuildQuery.h"


BuildQuery::BuildQuery():
    m_stratMap({
{SimpleStrategy, {"SimpleStrategy", "replication_factor"}},
{NetworkTopologyStrategy, {"NetworkTopologyStrategy", "dc1_name"}}
               })
{

}

BuildQuery &BuildQuery::createTable(const QString &table){
    m_query = QString("CREATE TABLE IF NOT EXISTS %1").arg(table);
    return *this;
}

BuildQuery &BuildQuery::dropTable(const QString &table)
{
    m_query = QString("DROP TABLE %1").arg(table);
    return *this;
}

BuildQuery &BuildQuery::insertInto(const QString &table)
{
    m_query = QString("INSERT INTO %1").arg(table);
    return *this;
}

BuildQuery &BuildQuery::createKeySpace(const QString &space)
{
    m_query = QString("CREATE KEYSPACE IF NOT EXISTS %1").arg(space);
    return *this;
}

BuildQuery &BuildQuery::dropKeySpace(const QString &space)
{
    m_query = QString("DROP KEYSPACE %1").arg(space);
    return *this;
}

/* TBD: Should be deprecated - Creating keyspaces and network topology should be
   defined externaly not in aspplication. App will know only the name of keyspace.*/
BuildQuery &BuildQuery::replication(BuildQuery::strategyType_t strategy,
                                      int stratInt, bool durableWrites)
{
    /*
    m_query = QString("%1  WITH REPLICATION = { 'class':'%2', '%3':%4 }"
                       " AND DURABLE_WRITES = %5").
            arg(m_query).
            arg(m_stratMap[strategy][0]).
            arg(m_stratMap[strategy][1]).
            arg( stratInt).
            arg(durableWrites);
*/
    m_query = QString("%1  WITH REPLICATION = { 'class':'%2', 'dc1':1,'dc2':1,'dc3':1, 'dc4':1 , 'dc5':1,'dc6':1,'dc7':1, 'dc8':1 }"
                       " AND DURABLE_WRITES = %5").
            arg(m_query).
            arg(m_stratMap[strategy][0]).
            arg(durableWrites);

    return *this;
}

BuildQuery &BuildQuery::lbrace(){
    m_query = QString("%1 (").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::values()
{
    m_query = QString("%1 VALUES").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::USING()
{
    m_query = QString("%1 USING").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::ttl(int ttl)
{
    m_query = QString("%1 TTL %2").arg(m_query).arg(ttl);
    return *this;
}

BuildQuery &BuildQuery::AND(const QString& cond)
{
    m_query = QString("%1 AND %2").arg(m_query).arg(cond);
    return *this;
}

BuildQuery &BuildQuery::comma()
{
    m_query = QString("%1,").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::timestamp(int64_t time)
{
    m_query = QString("%1 TIMESTAMP %2").arg(m_query).arg(time);
    return *this;
}

BuildQuery &BuildQuery::addStr(const QString &key)
{
    m_query = QString("%1 %2").arg(m_query).arg(key);
    return *this;
}

BuildQuery &BuildQuery::value(const QString &value)
{
    m_query = QString("%1 %2").arg(m_query).arg(value);
    return *this;
}

BuildQuery &BuildQuery::keyType(const QString &key, const QString &type) {
    m_query = QString("%1 %2 %3,").arg(m_query).arg(key).arg(type);
    return *this;
}

BuildQuery &BuildQuery::primaryKey(const QString &key) {
    m_query = QString("%1 PRIMARY KEY(%2)").arg(m_query).arg(key);
    return *this;
}

BuildQuery &BuildQuery::rbrace(){
    m_query = QString("%1 )").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::semicolon(){
    m_query = QString("%1;").arg(m_query);
    return *this;
}

BuildQuery &BuildQuery::select(const QString &filter)
{
    m_query = QString("SELECT %1").arg(filter);
    return *this;
}

BuildQuery &BuildQuery::from(const QString &table)
{
    m_query = QString("%1 FROM %2").arg(m_query).arg(table);
    return *this;
}

BuildQuery &BuildQuery::where(const QString &cond)
{
    m_query = QString("%1 WHERE %2").arg(m_query).arg(cond);
    return *this;
}

BuildQuery &BuildQuery::collectionSetType(const QString &key, const QString &type)
{
    m_query = QString("%1 %2 SET<%3>,").arg(m_query).arg(key).arg(type);
    return *this;
}


const QString &BuildQuery::query() const {
    return  m_query;
}

const char *BuildQuery::toConstChar()
{
    return m_query.toUtf8().constData();
}
