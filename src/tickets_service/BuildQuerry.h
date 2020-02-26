#ifndef BUILDQUERRY_H
#define BUILDQUERRY_H

#include <QString>
#include <QMap>
#include <QVector>

class BuildQuerry{
public:
    typedef enum {
        SimpleStrategy,
        NetworkTopologyStrategy
    }strategyType_t;

    BuildQuerry();
    BuildQuerry& createTable(const QString& table);
    BuildQuerry& dropTable(const QString& table);
    BuildQuerry& insertInto(const QString& table);
    BuildQuerry& createKeySpace(const QString& space);
    BuildQuerry& dropKeySpace(const QString& space);
    BuildQuerry& replication(strategyType_t strategy = SimpleStrategy, int stratInt = 1, bool durableWrites = true);
    BuildQuerry& lbrace();
    BuildQuerry& values();
    BuildQuerry& USING();
    BuildQuerry& ttl(int ttl);
    BuildQuerry& AND();
    BuildQuerry& comma();
    BuildQuerry& timestamp(int64_t time);
    BuildQuerry& key(const QString& key);
    BuildQuerry& value(const QString& value);
    BuildQuerry& keyType(const QString& key, const QString& type);
    BuildQuerry& primaryKey(const QString& key);
    BuildQuerry& rbrace();
    BuildQuerry& semicolon();
    const QString& querry() const;

protected:

    QString m_querry;
    const QMap<BuildQuerry::strategyType_t, QVector<QString>> m_stratMap;
};
#endif // BUILDQUERRY_H
