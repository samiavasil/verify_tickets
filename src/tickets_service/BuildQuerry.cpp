#include "BuildQuerry.h"


BuildQuerry::BuildQuerry()
{

}

BuildQuerry &BuildQuerry::create(){
    m_querry = QString("%1 CREATE").arg(m_querry);
    return *this;
}

BuildQuerry &BuildQuerry::table(const QString &table){
    m_querry = QString("%1 TABLE IF NOT EXISTS %2").arg(m_querry).arg(table);
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
