#ifndef APPENGINE_H
#define APPENGINE_H

#include <QObject>
#include <QQmlApplicationEngine>
#include "devices/DeviceManager.h"
#include "logging/AppLogger.h"
#include "storage/SettingsManager.h"

class AppEngine : public QObject
{
    Q_OBJECT
    QML_ELEMENT

public:
    explicit AppEngine(QObject *parent = nullptr);
    ~AppEngine();

    Q_INVOKABLE void initialize();
    Q_INVOKABLE void start();
    Q_INVOKABLE void shutdown();

    DeviceManager* deviceManager() const;
    AppLogger* logger() const;
    SettingsManager* settings() const;

signals:
    void initialized();
    void started();
    void shuttingDown();

private:
    void registerQmlTypes();
    void setupConnections();

    DeviceManager *m_deviceManager;
    AppLogger *m_logger;
    SettingsManager *m_settings;
    bool m_initialized;
};

#endif // APPENGINE_H
