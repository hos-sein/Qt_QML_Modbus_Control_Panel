#include "SettingsManager.h"
#include <QSettings>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , m_logLevel(2) // Default: Info
    , m_dataPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation))
{
}

int SettingsManager::logLevel() const
{
    return m_logLevel;
}

void SettingsManager::setLogLevel(int level)
{
    if (m_logLevel != level) {
        m_logLevel = level;
        emit logLevelChanged();
    }
}

QString SettingsManager::dataPath() const
{
    return m_dataPath;
}

void SettingsManager::setDataPath(const QString &path)
{
    if (m_dataPath != path) {
        m_dataPath = path;
        emit dataPathChanged();
    }
}

void SettingsManager::load()
{
    QSettings settings;
    m_logLevel = settings.value("logLevel", 2).toInt();
    m_dataPath = settings.value("dataPath", m_dataPath).toString();
}

void SettingsManager::save()
{
    QSettings settings;
    settings.setValue("logLevel", m_logLevel);
    settings.setValue("dataPath", m_dataPath);
}
