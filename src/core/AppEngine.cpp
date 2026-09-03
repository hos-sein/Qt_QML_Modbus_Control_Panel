#include "AppEngine.h"
#include <QQmlContext>
#include <QDir>

AppEngine::AppEngine(QObject *parent)
    : QObject(parent)
    , m_deviceManager(new DeviceManager(this))
    , m_logger(new AppLogger(this))
    , m_settings(new SettingsManager(this))
    , m_initialized(false)
{
}

AppEngine::~AppEngine()
{
    shutdown();
}

void AppEngine::initialize()
{
    if (m_initialized)
        return;

    registerQmlTypes();
    setupConnections();
    
    m_settings->load();
    m_logger->setLogLevel(static_cast<AppLogger::LogLevel>(m_settings->logLevel()));

    m_initialized = true;
    emit initialized();
}

void AppEngine::start()
{
    if (!m_initialized)
        initialize();

    // Load saved devices
    QString devicesPath = QDir::homePath() + "/.modbus_bms/devices.json";
    m_deviceManager->loadDevices(devicesPath);

    emit started();
}

void AppEngine::shutdown()
{
    emit shuttingDown();

    // Save devices
    QString devicesPath = QDir::homePath() + "/.modbus_bms/devices.json";
    QDir().mkpath(QDir::homePath() + "/.modbus_bms");
    m_deviceManager->saveDevices(devicesPath);

    m_settings->save();
}

DeviceManager* AppEngine::deviceManager() const
{
    return m_deviceManager;
}

AppLogger* AppEngine::logger() const
{
    return m_logger;
}

SettingsManager* AppEngine::settings() const
{
    return m_settings;
}

void AppEngine::registerQmlTypes()
{
    // QML types are registered via CMake qt_add_qml_module
}

void AppEngine::setupConnections()
{
    connect(m_logger, &AppLogger::logLevelChanged, this, [this]() {
        m_settings->setLogLevel(m_logger->logLevel());
    });
}
