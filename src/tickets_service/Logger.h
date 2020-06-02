#ifndef LOGGER_H
#define LOGGER_H

class PrivMsg;

class Logger
{
public:
    Logger();
    ~Logger();
private:
    PrivMsg* m_Msg;
};

#endif // LOGGER_H
