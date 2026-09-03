#include "SensorDevice.h"

SensorDevice::SensorDevice(QObject *parent)
    : Device(parent)
{
    m_type = "sensor";
}

bool SensorDevice::connectToDevice()
{
    setConnectionStatus(static_cast<int>(ConnectionStatus::Online));
    return true;
}

void SensorDevice::disconnectFromDevice()
{
    setConnectionStatus(static_cast<int>(ConnectionStatus::Offline));
}

bool SensorDevice::testConnection()
{
    return online();
}

void SensorDevice::readRegister(int address, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(functionCode);
}

void SensorDevice::writeRegister(int address, int value, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(value);
    Q_UNUSED(functionCode);
}
