#include "DeviceModel.h"

DeviceModel::DeviceModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int DeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_devices.size();
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_devices.size())
        return QVariant();

    Device *device = m_devices.at(index.row());

    switch (role) {
    case DeviceIdRole:
        return device->deviceId();
    case NameRole:
        return device->name();
    case TypeRole:
        return device->type();
    case ProtocolRole:
        return device->protocol();
    case ConnectionStatusRole:
        return device->connectionStatus();
    case OnlineRole:
        return device->online();
    case RoomRole:
        return device->room();
    case DescriptionRole:
        return device->description();
    case ChannelCountRole:
        return device->metaObject()->invokeMethod(device, "channelCount", Q_RETURN_ARG(int));
    case ChannelsRole:
        return device->registers();
    default:
        return QVariant();
    }
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[DeviceIdRole] = "deviceId";
    roles[NameRole] = "deviceName";
    roles[TypeRole] = "deviceType";
    roles[ProtocolRole] = "protocol";
    roles[ConnectionStatusRole] = "connectionStatus";
    roles[OnlineRole] = "online";
    roles[RoomRole] = "room";
    roles[DescriptionRole] = "description";
    roles[ChannelCountRole] = "channelCount";
    roles[ChannelsRole] = "channels";
    return roles;
}

void DeviceModel::addDevice(Device *device)
{
    if (!device)
        return;

    beginInsertRows(QModelIndex(), m_devices.size(), m_devices.size());
    m_devices.append(device);
    endInsertRows();

    connect(device, &Device::nameChanged, this, [this, device]() {
        int index = indexOfDevice(device->deviceId());
        if (index >= 0) {
            emit dataChanged(createIndex(index, 0), createIndex(index, 0), {NameRole});
        }
    });

    connect(device, &Device::connectionStatusChanged, this, [this, device]() {
        int index = indexOfDevice(device->deviceId());
        if (index >= 0) {
            emit dataChanged(createIndex(index, 0), createIndex(index, 0), {ConnectionStatusRole, OnlineRole});
        }
    });
}

void DeviceModel::removeDevice(const QString &deviceId)
{
    int index = indexOfDevice(deviceId);
    if (index < 0)
        return;

    beginRemoveRows(QModelIndex(), index, index);
    Device *device = m_devices.takeAt(index);
    device->deleteLater();
    endRemoveRows();
}

void DeviceModel::updateDevice(const QString &deviceId)
{
    int index = indexOfDevice(deviceId);
    if (index < 0)
        return;

    emit dataChanged(createIndex(index, 0), createIndex(index, 0));
}

Device* DeviceModel::getDevice(int index) const
{
    if (index < 0 || index >= m_devices.size())
        return nullptr;
    return m_devices.at(index);
}

Device* DeviceModel::getDeviceById(const QString &deviceId) const
{
    for (Device *device : m_devices) {
        if (device->deviceId() == deviceId)
            return device;
    }
    return nullptr;
}

int DeviceModel::indexOfDevice(const QString &deviceId) const
{
    for (int i = 0; i < m_devices.size(); ++i) {
        if (m_devices.at(i)->deviceId() == deviceId)
            return i;
    }
    return -1;
}
