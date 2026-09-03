#ifndef SWITCHDEVICE_H
#define SWITCHDEVICE_H

#include "Device.h"
#include <QVector>

/**
 * @brief Represents a multi-channel switch device
 * 
 * A switch device typically has multiple channels (e.g., 4-gang switch).
 * Each channel has separate command and state registers.
 */
class SwitchDevice : public Device
{
    Q_OBJECT

    Q_PROPERTY(int channelCount READ channelCount NOTIFY channelCountChanged)
    Q_PROPERTY(QVariantList channels READ channels NOTIFY channelsChanged)

public:
    explicit SwitchDevice(QObject *parent = nullptr);
    ~SwitchDevice() override;

    // Connection methods
    bool connectToDevice() override;
    void disconnectFromDevice() override;
    bool testConnection() override;

    // Read/Write operations
    void readRegister(int address, int functionCode) override;
    void writeRegister(int address, int value, int functionCode) override;

    // Channel-specific operations
    Q_INVOKABLE void setChannelState(int channel, bool state);
    Q_INVOKABLE bool getChannelState(int channel) const;
    Q_INVOKABLE void toggleChannel(int channel);

    int channelCount() const;
    void setChannelCount(int count);

    QVariantList channels() const;

    // Serialization
    QJsonObject toJson() const override;
    void fromJson(const QJsonObject &json) override;

signals:
    void channelCountChanged();
    void channelsChanged();
    void channelStateChanged(int channel, bool state);

private:
    struct Channel {
        int commandAddress;
        int stateAddress;
        int functionCode;
        bool currentState;
        bool commandedState;
    };

    QVector<Channel> m_channels;
};

#endif // SWITCHDEVICE_H
