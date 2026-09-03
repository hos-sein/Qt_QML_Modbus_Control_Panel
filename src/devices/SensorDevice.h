#ifndef SENSORDEVICE_H
#define SENSORDEVICE_H

#include "Device.h"

class SensorDevice : public Device
{
    Q_OBJECT
public:
    explicit SensorDevice(QObject *parent = nullptr);
    
    bool connectToDevice() override;
    void disconnectFromDevice() override;
    bool testConnection() override;
    void readRegister(int address, int functionCode) override;
    void writeRegister(int address, int value, int functionCode) override;
};

#endif // SENSORDEVICE_H
