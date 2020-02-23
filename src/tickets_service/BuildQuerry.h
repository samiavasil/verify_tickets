#ifndef BUILDQUERRY_H
#define BUILDQUERRY_H

#include <QString>

class BuildQuerry{
public:
    BuildQuerry();
    BuildQuerry& create();
    BuildQuerry& table(const QString& table);
    BuildQuerry& lbrace();
    BuildQuerry& keyType(const QString& key, const QString& type);
    BuildQuerry& primaryKey(const QString& key);
    BuildQuerry& rbrace();
    BuildQuerry& semicolon();
    const QString &querry() const;

protected:
    QString m_querry;
};
#endif // BUILDQUERRY_H
