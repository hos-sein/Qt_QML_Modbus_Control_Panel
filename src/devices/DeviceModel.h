#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include "devices/Device.h"

class DeviceModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT

public:
    enum DeviceRoles {
        DeviceIdRole = Qt::UserRole + 1,
        NameRole,
        TypeRole,
        ProtocolRole,
        ConnectionStatusRole,
        OnlineRole,
        RoomRole,
        DescriptionRole,
        ChannelCountRole,
        ChannelsRole
    };

    explicit DeviceModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addDevice(Device *device);
    Q_INVOKABLE void removeDevice(const QString &deviceId);
    Q_INVOKABLE void updateDevice(const QString &deviceId);
    Q_INVOKABLE Device* getDevice(int index) const;
    Q_INVOKABLE Device* getDeviceById(const QString &deviceId) const;
    Q_INVOKABLE int indexOfDevice(const QString &deviceId) const;

private:
    QList<Device*> m_devices;
};

#endif // DEVICEMODEL_H
