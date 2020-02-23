#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QSettings>
#include <QString>

class Configurator
{
public:
    const Configurator& Instance();

protected:
    Configurator();
protected:
    QSettings m_cfg;
};

#endif // CONFIGURATOR_H
