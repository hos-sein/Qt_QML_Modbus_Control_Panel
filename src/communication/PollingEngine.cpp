#include "PollingEngine.h"
#include "devices/DeviceManager.h"

PollingEngine::PollingEngine(QObject *parent)
    : QObject(parent)
    , m_deviceManager(nullptr)
    , m_pollTimer(new QTimer(this))
    , m_running(false)
    , m_currentPollIndex(0)
{
    connect(m_pollTimer, &QTimer::timeout, this, &PollingEngine::onPollTimeout);
    m_pollTimer->setSingleShot(false);
}

void PollingEngine::setDeviceManager(DeviceManager *manager)
{
    m_deviceManager = manager;
}

void PollingEngine::start()
{
    if (m_running)
        return;
    
    m_running = true;
    m_pollTimer->start(100); // Base polling interval
    emit pollStarted();
}

void PollingEngine::stop()
{
    if (!m_running)
        return;
    
    m_running = false;
    m_pollTimer->stop();
}

bool PollingEngine::isRunning() const
{
    return m_running;
}

void PollingEngine::addPollingTask(const QString &deviceId, const PollingConfig &config)
{
    m_pollingConfigs[deviceId] = config;
}

void PollingEngine::removePollingTask(const QString &deviceId)
{
    m_pollingConfigs.remove(deviceId);
}

void PollingEngine::enablePolling(const QString &deviceId, bool enabled)
{
    if (m_pollingConfigs.contains(deviceId)) {
        m_pollingConfigs[deviceId].enabled = enabled;
    }
}

void PollingEngine::onPollTimeout()
{
    if (m_pollingConfigs.isEmpty())
        return;
    
    QStringList deviceIds = m_pollingConfigs.keys();
    
    // Round-robin through all polling tasks
    for (int i = 0; i < deviceIds.size(); ++i) {
        int index = (m_currentPollIndex + i) % deviceIds.size();
        const QString &deviceId = deviceIds[index];
        const PollingConfig &config = m_pollingConfigs[deviceId];
        
        if (config.enabled) {
            executePoll(deviceId, config);
            break;
        }
    }
    
    m_currentPollIndex = (m_currentPollIndex + 1) % deviceIds.size();
}

void PollingEngine::executePoll(const QString &deviceId, const PollingConfig &config)
{
    Q_UNUSED(config);
    
    if (!m_deviceManager)
        return;
    
    // TODO: Implement actual polling logic using DeviceManager
    // This would read registers from the device at the configured interval
}
