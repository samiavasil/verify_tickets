#include "BuildQuerry.h"


BuildQuerry::BuildQuerry():
    m_stratMap({
                {SimpleStrategy, {"SimpleStrategy", "replication_factor"}},
                {NetworkTopologyStrategy, {"NetworkTopologyStrategy", "dc1_name"}}
               })
{

}


BuildQuerry &BuildQuerry::createTable(const QString &table){
    m_querry = QString("CREATE TABLE IF NOT EXISTS %2").arg(m_querry).arg(table);
    return *this;
}

BuildQuerry &BuildQuerry::createKeySpace(const QString &space)
{
    m_querry = QString("CREATE KEYSPACE %1 IF NOT EXISTS %2").arg(m_querry).arg(space);
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
    m_querry = QString("%1(").arg(m_querry);
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
    m_querry = QString("%1)").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::semicolon(){
    m_querry = QString("%1;").arg(m_querry);
    return *this;
}

const QString &BuildQuerry::querry() const {
    return  m_querry;
}
