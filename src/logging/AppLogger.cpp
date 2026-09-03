#include "AppLogger.h"

AppLogger *AppLogger::s_instance = nullptr;

AppLogger::AppLogger(QObject *parent)
    : QObject(parent)
    , m_logLevel(LogLevel::Info)
    , m_maxLogCount(500)
{
    if (!s_instance) {
        s_instance = this;
    }
}

AppLogger* AppLogger::instance()
{
    return s_instance;
}

void AppLogger::log(LogLevel level, const QString &message, const QString &source)
{
    if (level > m_logLevel)
        return;

    LogEntry entry;
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = level;
    entry.message = message;
    entry.source = source;

    m_logs.append(entry);

    // Trim log if too large
    while (m_logs.size() > m_maxLogCount) {
        m_logs.removeFirst();
    }

    QVariant variant;
    variant.setValue<QMap<QString, QVariant>>({
        {"timestamp", entry.timestamp.toString(Qt::ISODate)},
        {"level", static_cast<int>(level)},
        {"message", message},
        {"source", source}
    });

    emit newLog(variant);
}

void AppLogger::error(const QString &message, const QString &source)
{
    log(LogLevel::Error, message, source);
}

void AppLogger::warning(const QString &message, const QString &source)
{
    log(LogLevel::Warning, message, source);
}

void AppLogger::info(const QString &message, const QString &source)
{
    log(LogLevel::Info, message, source);
}

void AppLogger::debug(const QString &message, const QString &source)
{
    log(LogLevel::Debug, message, source);
}

void AppLogger::protocol(const QString &message, const QString &source)
{
    log(LogLevel::Protocol, message, source);
}

QVector<QVariant> AppLogger::getLogs(int maxCount) const
{
    QVector<QVariant> result;
    int start = qMax(0, m_logs.size() - maxCount);
    
    for (int i = start; i < m_logs.size(); ++i) {
        const LogEntry &entry = m_logs[i];
        result.append(QVariant::fromValue<QMap<QString, QVariant>>({
            {"timestamp", entry.timestamp.toString(Qt::ISODate)},
            {"level", static_cast<int>(entry.level)},
            {"message", entry.message},
            {"source", entry.source}
        }));
    }
    
    return result;
}

void AppLogger::clear()
{
    m_logs.clear();
}

void AppLogger::setLogLevel(LogLevel level)
{
    if (m_logLevel != level) {
        m_logLevel = level;
        emit logLevelChanged();
    }
}

AppLogger::LogLevel AppLogger::logLevel() const
{
    return m_logLevel;
}
