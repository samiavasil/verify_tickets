#include "Configurator.h"

const Configurator &Configurator::Instance()
{
    static Configurator m_cfg;
    return m_cfg;
}

Configurator::Configurator()
{

}
