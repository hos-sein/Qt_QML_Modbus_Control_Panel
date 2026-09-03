#ifndef RELAYDEVICE_H
#define RELAYDEVICE_H

#include "Device.h"

class RelayDevice : public Device
{
    Q_OBJECT
public:
    explicit RelayDevice(QObject *parent = nullptr);
    
    bool connectToDevice() override;
    void disconnectFromDevice() override;
    bool testConnection() override;
    void readRegister(int address, int functionCode) override;
    void writeRegister(int address, int value, int functionCode) override;
};

#endif // RELAYDEVICE_H
