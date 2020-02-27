#include "Configurator.h"

const Configurator &Configurator::Instance()
{
    static Configurator m_inst;
    return m_inst;
}

Configurator::Configurator()
{

}
