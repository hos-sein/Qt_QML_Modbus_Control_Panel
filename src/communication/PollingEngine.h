#ifndef POLLINGENGINE_H
#define POLLINGENGINE_H

#include <QObject>
#include <QTimer>
#include <QMap>

class DeviceManager;
class ModbusClient;

struct PollingConfig {
    int intervalMs;
    int address;
    int functionCode;
    int quantity;
    bool enabled;
};

class PollingEngine : public QObject
{
    Q_OBJECT
public:
    explicit PollingEngine(QObject *parent = nullptr);
    
    void setDeviceManager(DeviceManager *manager);
    void start();
    void stop();
    bool isRunning() const;
    
    void addPollingTask(const QString &deviceId, const PollingConfig &config);
    void removePollingTask(const QString &deviceId);
    void enablePolling(const QString &deviceId, bool enabled);

signals:
    void pollStarted();
    void pollCompleted();
    void pollingError(const QString &deviceId, const QString &error);

private slots:
    void onPollTimeout();

private:
    void executePoll(const QString &deviceId, const PollingConfig &config);
    
    DeviceManager *m_deviceManager;
    QTimer *m_pollTimer;
    QMap<QString, PollingConfig> m_pollingConfigs;
    bool m_running;
    int m_currentPollIndex;
};

#endif // POLLINGENGINE_H
