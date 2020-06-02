#include <QString>
#include "Logger.h"
#include "MqttManager.h"
#include "Configurator.h"

class PrivMsg{
public:
    static void myMessageOutput(QtMsgType type, const QMessageLogContext &, const QString & msg)
    {
        QString out_dbg;
        bool log = false;

        switch (type) {
        case QtDebugMsg: {
            if (Configurator::Instance().LogsLevel() == Configurator::DEBUG) {
                out_dbg = QString("Debug: %1").arg(msg);
                log = true;
            }
            break;
        }
        case QtWarningMsg:
            out_dbg = QString("Warning: %1").arg(msg);
            log = true;
            break;
        case QtCriticalMsg:
            out_dbg = QString("Critical: %1").arg(msg);
            log = true;
            break;
        case QtFatalMsg:
            out_dbg = QString("Fatal: %1").arg(msg);
            log = true;
            break;
        default:
            break;
        }

        if (log) {
            if (Configurator::Instance().Mqtt().enbl_logs) {
                QString topic = Configurator::Instance().Mqtt().log_topic;
                MqttManager::Instance().publish(QMqttTopicName(topic), out_dbg.toUtf8(), 1, false);
            }
            fprintf(stderr, "%s\n", out_dbg.toStdString().c_str());
        }
    }
};

Logger::Logger()
{
    m_Msg = new PrivMsg;
    qInstallMessageHandler(m_Msg->myMessageOutput);
}

Logger::~Logger()
{
    delete m_Msg;
}
