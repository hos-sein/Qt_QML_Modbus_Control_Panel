#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QMap>
#include "devices/Device.h"
#include "devices/DeviceModel.h"

class ModbusClient;
class CommandQueue;
class PollingEngine;

class DeviceManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(DeviceModel* deviceModel READ deviceModel CONSTANT)
    Q_PROPERTY(int deviceCount READ deviceCount NOTIFY deviceCountChanged)

public:
    explicit DeviceManager(QObject *parent = nullptr);
    ~DeviceManager();

    DeviceModel* deviceModel() const;
    int deviceCount() const;

    Q_INVOKABLE void addDevice(const QString &deviceId, const QString &name, 
                               const QString &type, const QString &protocol);
    Q_INVOKABLE void removeDevice(const QString &deviceId);
    Q_INVOKABLE Device* getDevice(const QString &deviceId);
    
    Q_INVOKABLE bool connectDevice(const QString &deviceId);
    Q_INVOKABLE void disconnectDevice(const QString &deviceId);
    Q_INVOKABLE bool testConnection(const QString &deviceId);
    
    Q_INVOKABLE void writeValue(const QString &deviceId, int address, int value, int functionCode);
    Q_INVOKABLE void readValue(const QString &deviceId, int address, int functionCode);

    Q_INVOKABLE void saveDevices(const QString &filePath);
    Q_INVOKABLE void loadDevices(const QString &filePath);

signals:
    void deviceCountChanged();
    void deviceAdded(const QString &deviceId);
    void deviceRemoved(const QString &deviceId);
    void deviceStateChanged(const QString &deviceId, int state);
    void errorOccurred(const QString &deviceId, const QString &error);

private:
    Device* createDevice(const QString &type);
    
    QMap<QString, Device*> m_devices;
    DeviceModel *m_deviceModel;
    CommandQueue *m_commandQueue;
    PollingEngine *m_pollingEngine;
};

#endif // DEVICEMANAGER_H
