#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QSettings>
#include <QString>

class Configurator
{
public:
    static Configurator &Instance();
    int site_id() const;
    int protocol();
    QString hosts();
    QString user();
    QString password();
protected:
    Configurator();
protected:
    QSettings m_cfg;
};

#endif // CONFIGURATOR_H
