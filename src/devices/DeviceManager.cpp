#include "DeviceManager.h"
#include "SwitchDevice.h"
#include "RelayDevice.h"
#include "SensorDevice.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>

// Forward declarations (will be implemented later)
class CommandQueue;
class PollingEngine;

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
    , m_deviceModel(new DeviceModel(this))
    , m_commandQueue(nullptr) // TODO: Initialize when CommandQueue is ready
    , m_pollingEngine(nullptr) // TODO: Initialize when PollingEngine is ready
{
}

DeviceManager::~DeviceManager()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

DeviceModel* DeviceManager::deviceModel() const
{
    return m_deviceModel;
}

int DeviceManager::deviceCount() const
{
    return m_devices.size();
}

void DeviceManager::addDevice(const QString &deviceId, const QString &name, 
                              const QString &type, const QString &protocol)
{
    if (m_devices.contains(deviceId)) {
        emit errorOccurred(deviceId, tr("Device already exists"));
        return;
    }

    Device *device = createDevice(type);
    if (!device) {
        emit errorOccurred(deviceId, tr("Unknown device type: %1").arg(type));
        return;
    }

    device->setDeviceId(deviceId);
    device->setName(name);
    device->setType(type);
    device->setProtocol(protocol);

    m_devices[deviceId] = device;
    m_deviceModel->addDevice(device);

    connect(device, &Device::connectionStatusChanged, this, [this, deviceId, device]() {
        emit deviceStateChanged(deviceId, device->connectionStatus());
    });

    connect(device, &Device::errorOccurred, this, [this, deviceId](const QString &error) {
        emit errorOccurred(deviceId, error);
    });

    emit deviceAdded(deviceId);
    emit deviceCountChanged();
}

void DeviceManager::removeDevice(const QString &deviceId)
{
    if (!m_devices.contains(deviceId))
        return;

    Device *device = m_devices.take(deviceId);
    m_deviceModel->removeDevice(deviceId);
    
    device->deleteLater();

    emit deviceRemoved(deviceId);
    emit deviceCountChanged();
}

Device* DeviceManager::getDevice(const QString &deviceId)
{
    return m_devices.value(deviceId, nullptr);
}

bool DeviceManager::connectDevice(const QString &deviceId)
{
    Device *device = getDevice(deviceId);
    if (!device) {
        emit errorOccurred(deviceId, tr("Device not found"));
        return false;
    }

    return device->connectToDevice();
}

void DeviceManager::disconnectDevice(const QString &deviceId)
{
    Device *device = getDevice(deviceId);
    if (!device)
        return;

    device->disconnectFromDevice();
}

bool DeviceManager::testConnection(const QString &deviceId)
{
    Device *device = getDevice(deviceId);
    if (!device)
        return false;

    return device->testConnection();
}

void DeviceManager::writeValue(const QString &deviceId, int address, int value, int functionCode)
{
    Device *device = getDevice(deviceId);
    if (!device)
        return;

    device->writeRegister(address, value, functionCode);
}

void DeviceManager::readValue(const QString &deviceId, int address, int functionCode)
{
    Device *device = getDevice(deviceId);
    if (!device)
        return;

    device->readRegister(address, functionCode);
}

void DeviceManager::saveDevices(const QString &filePath)
{
    QJsonArray devicesArray;

    for (Device *device : m_devices) {
        devicesArray.append(device->toJson());
    }

    QJsonObject root;
    root["devices"] = devicesArray;

    QJsonDocument doc(root);

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void DeviceManager::loadDevices(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();
    QJsonArray devicesArray = root["devices"].toArray();

    for (const QJsonValue &value : devicesArray) {
        QJsonObject deviceJson = value.toObject();
        QString deviceId = deviceJson["deviceId"].toString();
        QString name = deviceJson["name"].toString();
        QString type = deviceJson["type"].toString();
        QString protocol = deviceJson["protocol"].toString();

        addDevice(deviceId, name, type, protocol);

        Device *device = getDevice(deviceId);
        if (device) {
            device->fromJson(deviceJson);
        }
    }
}

Device* DeviceManager::createDevice(const QString &type)
{
    if (type == "switch")
        return new SwitchDevice(this);
    else if (type == "relay")
        return new RelayDevice(this);
    else if (type == "sensor")
        return new SensorDevice(this);
    else
        return new Device(this); // Generic device
}
