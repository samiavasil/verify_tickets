#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <QSettings>
#include <QString>

typedef struct {
    QString host;
    quint16 port;
    QString user;
    QString password;
    QString topic;
} ComarCfg;

class Configurator
{
public:
    static Configurator &Instance();
    int site_id() const;
    int protocol();
    QString hosts();
    QString user();
    QString password();
    QString keyspace(); 
    const ComarCfg &Comar() const;
protected:
    Configurator();
protected:
    QSettings m_cfg;
    ComarCfg  m_Comar;
};

#endif // CONFIGURATOR_H
