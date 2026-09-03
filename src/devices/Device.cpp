#include "Device.h"

Device::Device(QObject *parent)
    : QObject(parent)
    , m_deviceId("")
    , m_name("Unknown Device")
    , m_type("generic")
    , m_protocol("modbus_tcp")
    , m_connectionStatus(static_cast<int>(ConnectionStatus::Offline))
    , m_room("")
    , m_description("")
{
}

Device::~Device()
{
}

QString Device::deviceId() const
{
    return m_deviceId;
}

void Device::setDeviceId(const QString &deviceId)
{
    if (m_deviceId != deviceId) {
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }
}

QString Device::name() const
{
    return m_name;
}

void Device::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        emit nameChanged();
    }
}

QString Device::type() const
{
    return m_type;
}

void Device::setType(const QString &type)
{
    if (m_type != type) {
        m_type = type;
        emit typeChanged();
    }
}

QString Device::protocol() const
{
    return m_protocol;
}

void Device::setProtocol(const QString &protocol)
{
    if (m_protocol != protocol) {
        m_protocol = protocol;
        emit protocolChanged();
    }
}

int Device::connectionStatus() const
{
    return m_connectionStatus;
}

void Device::setConnectionStatus(int status)
{
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged();
        emit onlineChanged();
    }
}

bool Device::online() const
{
    return m_connectionStatus == static_cast<int>(ConnectionStatus::Online);
}

QString Device::room() const
{
    return m_room;
}

void Device::setRoom(const QString &room)
{
    if (m_room != room) {
        m_room = room;
        emit roomChanged();
    }
}

QString Device::description() const
{
    return m_description;
}

void Device::setDescription(const QString &description)
{
    if (m_description != description) {
        m_description = description;
        emit descriptionChanged();
    }
}

QVariantList Device::registers() const
{
    return m_registers;
}

void Device::setRegisters(const QVariantList &registers)
{
    if (m_registers != registers) {
        m_registers = registers;
        emit registersChanged();
    }
}

QJsonObject Device::toJson() const
{
    QJsonObject json;
    json["deviceId"] = m_deviceId;
    json["name"] = m_name;
    json["type"] = m_type;
    json["protocol"] = m_protocol;
    json["room"] = m_room;
    json["description"] = m_description;
    return json;
}

void Device::fromJson(const QJsonObject &json)
{
    m_deviceId = json["deviceId"].toString();
    m_name = json["name"].toString();
    m_type = json["type"].toString();
    m_protocol = json["protocol"].toString();
    m_room = json["room"].toString();
    m_description = json["description"].toString();
}
