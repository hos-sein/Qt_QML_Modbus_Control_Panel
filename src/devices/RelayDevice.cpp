#include "RelayDevice.h"

RelayDevice::RelayDevice(QObject *parent)
    : Device(parent)
{
    m_type = "relay";
}

bool RelayDevice::connectToDevice()
{
    setConnectionStatus(static_cast<int>(ConnectionStatus::Online));
    return true;
}

void RelayDevice::disconnectFromDevice()
{
    setConnectionStatus(static_cast<int>(ConnectionStatus::Offline));
}

bool RelayDevice::testConnection()
{
    return online();
}

void RelayDevice::readRegister(int address, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(functionCode);
}

void RelayDevice::writeRegister(int address, int value, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(value);
    Q_UNUSED(functionCode);
}
