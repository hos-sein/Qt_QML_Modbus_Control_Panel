#ifndef APPLOGGER_H
#define APPLOGGER_H

#include <QObject>
#include <QDateTime>
#include <QVector>

class AppLogger : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum class LogLevel {
        Error = 0,
        Warning = 1,
        Info = 2,
        Debug = 3,
        Protocol = 4
    };
    Q_ENUM(LogLevel)

    struct LogEntry {
        QDateTime timestamp;
        LogLevel level;
        QString message;
        QString source;
    };

    explicit AppLogger(QObject *parent = nullptr);
    
    static AppLogger* instance();
    
    Q_INVOKABLE void log(LogLevel level, const QString &message, const QString &source = QString());
    Q_INVOKABLE void error(const QString &message, const QString &source = QString());
    Q_INVOKABLE void warning(const QString &message, const QString &source = QString());
    Q_INVOKABLE void info(const QString &message, const QString &source = QString());
    Q_INVOKABLE void debug(const QString &message, const QString &source = QString());
    Q_INVOKABLE void protocol(const QString &message, const QString &source = QString());
    
    Q_INVOKABLE QVector<QVariant> getLogs(int maxCount = 100) const;
    Q_INVOKABLE void clear();
    
    void setLogLevel(LogLevel level);
    LogLevel logLevel() const;

signals:
    void newLog(const QVariant &entry);
    void logLevelChanged();

private:
    static AppLogger *s_instance;
    
    QVector<LogEntry> m_logs;
    LogLevel m_logLevel;
    int m_maxLogCount;
};

#endif // APPLOGGER_H
