#include "SwitchDevice.h"
#include <QJsonObject>
#include <QJsonArray>

SwitchDevice::SwitchDevice(QObject *parent)
    : Device(parent)
{
    m_type = "switch";
    setChannelCount(4); // Default 4-channel switch
}

SwitchDevice::~SwitchDevice()
{
}

bool SwitchDevice::connectToDevice()
{
    // TODO: Implement actual connection logic
    setConnectionStatus(static_cast<int>(ConnectionStatus::Online));
    return true;
}

void SwitchDevice::disconnectFromDevice()
{
    // TODO: Implement actual disconnection logic
    setConnectionStatus(static_cast<int>(ConnectionStatus::Offline));
}

bool SwitchDevice::testConnection()
{
    // TODO: Implement actual test connection logic
    return online();
}

void SwitchDevice::readRegister(int address, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(functionCode);
    // TODO: Implement actual read logic
}

void SwitchDevice::writeRegister(int address, int value, int functionCode)
{
    Q_UNUSED(address);
    Q_UNUSED(value);
    Q_UNUSED(functionCode);
    // TODO: Implement actual write logic
}

void SwitchDevice::setChannelState(int channel, bool state)
{
    if (channel >= 0 && channel < m_channels.size()) {
        m_channels[channel].commandedState = state;
        emit channelStateChanged(channel, state);
        
        // TODO: Send Modbus command to update state
        // This would typically go through the CommandQueue
    }
}

bool SwitchDevice::getChannelState(int channel) const
{
    if (channel >= 0 && channel < m_channels.size()) {
        return m_channels[channel].currentState;
    }
    return false;
}

void SwitchDevice::toggleChannel(int channel)
{
    if (channel >= 0 && channel < m_channels.size()) {
        bool newState = !m_channels[channel].currentState;
        setChannelState(channel, newState);
    }
}

int SwitchDevice::channelCount() const
{
    return m_channels.size();
}

void SwitchDevice::setChannelCount(int count)
{
    if (count <= 0) {
        count = 1;
    }
    
    if (m_channels.size() != count) {
        m_channels.resize(count);
        
        // Initialize channels with default addresses
        for (int i = 0; i < count; ++i) {
            m_channels[i].commandAddress = i * 2;     // Even addresses for commands
            m_channels[i].stateAddress = i * 2 + 1;   // Odd addresses for states
            m_channels[i].functionCode = 0x05;        // Write single coil
            m_channels[i].currentState = false;
            m_channels[i].commandedState = false;
        }
        
        emit channelCountChanged();
        emit channelsChanged();
    }
}

QVariantList SwitchDevice::channels() const
{
    QVariantList list;
    for (int i = 0; i < m_channels.size(); ++i) {
        QVariantMap channelMap;
        channelMap["index"] = i;
        channelMap["currentState"] = m_channels[i].currentState;
        channelMap["commandedState"] = m_channels[i].commandedState;
        channelMap["commandAddress"] = m_channels[i].commandAddress;
        channelMap["stateAddress"] = m_channels[i].stateAddress;
        list.append(channelMap);
    }
    return list;
}

QJsonObject SwitchDevice::toJson() const
{
    QJsonObject json = Device::toJson();
    
    QJsonArray channelsArray;
    for (int i = 0; i < m_channels.size(); ++i) {
        QJsonObject channelJson;
        channelJson["commandAddress"] = m_channels[i].commandAddress;
        channelJson["stateAddress"] = m_channels[i].stateAddress;
        channelJson["functionCode"] = m_channels[i].functionCode;
        channelsArray.append(channelJson);
    }
    
    json["channels"] = channelsArray;
    json["channelCount"] = m_channels.size();
    
    return json;
}

void SwitchDevice::fromJson(const QJsonObject &json)
{
    Device::fromJson(json);
    
    int channelCount = json["channelCount"].toInt(4);
    setChannelCount(channelCount);
    
    QJsonArray channelsArray = json["channels"].toArray();
    for (int i = 0; i < channelsArray.size() && i < m_channels.size(); ++i) {
        QJsonObject channelJson = channelsArray[i].toObject();
        m_channels[i].commandAddress = channelJson["commandAddress"].toInt(i * 2);
        m_channels[i].stateAddress = channelJson["stateAddress"].toInt(i * 2 + 1);
        m_channels[i].functionCode = channelJson["functionCode"].toInt(0x05);
    }
}
