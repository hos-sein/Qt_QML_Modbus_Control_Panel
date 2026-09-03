#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>

class SettingsManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int logLevel READ logLevel WRITE setLogLevel NOTIFY logLevelChanged)
    Q_PROPERTY(QString dataPath READ dataPath WRITE setDataPath NOTIFY dataPathChanged)
    
public:
    explicit SettingsManager(QObject *parent = nullptr);
    
    int logLevel() const;
    void setLogLevel(int level);
    
    QString dataPath() const;
    void setDataPath(const QString &path);
    
    Q_INVOKABLE void load();
    Q_INVOKABLE void save();

signals:
    void logLevelChanged();
    void dataPathChanged();

private:
    int m_logLevel;
    QString m_dataPath;
};

#endif // SETTINGSMANAGER_H
