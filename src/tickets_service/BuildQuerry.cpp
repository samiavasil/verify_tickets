#include "BuildQuerry.h"


BuildQuerry::BuildQuerry():
    m_stratMap({
{SimpleStrategy, {"SimpleStrategy", "replication_factor"}},
{NetworkTopologyStrategy, {"NetworkTopologyStrategy", "dc1_name"}}
               })
{

}

BuildQuerry &BuildQuerry::createTable(const QString &table){
    m_querry = QString("CREATE TABLE IF NOT EXISTS %1").arg(table);
    return *this;
}

BuildQuerry &BuildQuerry::dropTable(const QString &table)
{
    m_querry = QString("DROP TABLE %1").arg(table);
    return *this;
}

BuildQuerry &BuildQuerry::insertInto(const QString &table)
{
    m_querry = QString("INSERT INTO %1").arg(table);
    return *this;
}

BuildQuerry &BuildQuerry::createKeySpace(const QString &space)
{
    m_querry = QString("CREATE KEYSPACE IF NOT EXISTS %1").arg(space);
    return *this;
}

BuildQuerry &BuildQuerry::dropKeySpace(const QString &space)
{
    m_querry = QString("DROP KEYSPACE %1").arg(space);
    return *this;
}

BuildQuerry &BuildQuerry::replication(BuildQuerry::strategyType_t strategy,
                                      int stratInt, bool durableWrites)
{
    m_querry = QString("%1  WITH REPLICATION = { 'class':'%2', '%3':%4 }"
                       " AND DURABLE_WRITES = %5").
            arg(m_querry).
            arg(m_stratMap[strategy][0]).
            arg(m_stratMap[strategy][1]).
            arg( stratInt).
            arg(durableWrites);
    return *this;
}

BuildQuerry &BuildQuerry::lbrace(){
    m_querry = QString("%1 (").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::values()
{
    m_querry = QString("%1 VALUES").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::USING()
{
    m_querry = QString("%1 USING").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::ttl(int ttl)
{
    m_querry = QString("%1 TTL %2").arg(m_querry).arg(ttl);
    return *this;
}

BuildQuerry &BuildQuerry::AND()
{
    m_querry = QString("%1 AND").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::comma()
{
    m_querry = QString("%1,").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::timestamp(int64_t time)
{
    m_querry = QString("%1 TIMESTAMP %2").arg(m_querry).arg(time);
    return *this;
}

BuildQuerry &BuildQuerry::key(const QString &key)
{
    m_querry = QString("%1 %2").arg(m_querry).arg(key);
    return *this;
}

BuildQuerry &BuildQuerry::value(const QString &value)
{
    m_querry = QString("%1 %2").arg(m_querry).arg(value);
    return *this;
}

BuildQuerry &BuildQuerry::keyType(const QString &key, const QString &type) {
    m_querry = QString("%1 %2 %3,").arg(m_querry).arg(key).arg(type);
    return *this;
}

BuildQuerry &BuildQuerry::primaryKey(const QString &key) {
    m_querry = QString("%1 PRIMARY KEY(%2)").arg(m_querry).arg(key);
    return *this;
}

BuildQuerry &BuildQuerry::rbrace(){
    m_querry = QString("%1 )").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::semicolon(){
    m_querry = QString("%1;").arg(m_querry);
    return *this;
}

const QString &BuildQuerry::querry() const {
    return  m_querry;
}
