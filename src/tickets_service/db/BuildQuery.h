#ifndef BUILDQUERY_H
#define BUILDQUERY_H

#include <QString>
#include <QMap>
#include <QVector>

class BuildQuery{
public:
    typedef enum {
        SimpleStrategy,
        NetworkTopologyStrategy
    }strategyType_t;

    BuildQuery();
    BuildQuery& createTable(const QString& table);
    BuildQuery& dropTable(const QString& table);
    BuildQuery& insertInto(const QString& table);
    BuildQuery& createKeySpace(const QString& space);
    BuildQuery& dropKeySpace(const QString& space);
    BuildQuery& replication(strategyType_t strategy = SimpleStrategy, int stratInt = 1, bool durableWrites = true);
    BuildQuery& lbrace();
    BuildQuery& values();
    BuildQuery& USING();
    BuildQuery& ttl(int ttl);
    BuildQuery& AND(const QString& cond);
    BuildQuery& comma();
    BuildQuery& timestamp(int64_t time);
    BuildQuery& addStr(const QString& addStr);
    BuildQuery& value(const QString& value);
    BuildQuery& keyType(const QString& addStr, const QString& type);
    BuildQuery& primaryKey(const QString& addStr);
    BuildQuery& rbrace();
    BuildQuery& semicolon();
    BuildQuery& select(const QString& filter);
    BuildQuery& from(const QString& table);
    BuildQuery& where(const QString& cond);
    BuildQuery& collectionSetType(const QString &key, const QString& type);

    const QString& query() const;
    const char* toConstChar();
protected:

    QString m_query;
    const QMap<BuildQuery::strategyType_t, QVector<QString>> m_stratMap;
};
#endif // BUILDQUERY_H
