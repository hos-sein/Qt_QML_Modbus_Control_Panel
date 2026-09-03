#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include "modbus/ModbusClient.h"

class Register;

/**
 * @brief Base class for all Modbus devices
 * 
 * Represents a physical or logical device in the BMS system.
 */
class Device : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString protocol READ protocol WRITE setProtocol NOTIFY protocolChanged)
    Q_PROPERTY(int connectionStatus READ connectionStatus NOTIFY connectionStatusChanged)
    Q_PROPERTY(bool online READ online NOTIFY onlineChanged)
    Q_PROPERTY(QString room READ room WRITE setRoom NOTIFY roomChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(QVariantList registers READ registers NOTIFY registersChanged)

public:
    enum class Protocol {
        ModbusTcp,
        ModbusRtu
    };
    Q_ENUM(Protocol)

    enum class ConnectionStatus {
        Online,
        Offline,
        Connecting,
        Error
    };
    Q_ENUM(ConnectionStatus)

    explicit Device(QObject *parent = nullptr);
    virtual ~Device();

    // Properties
    QString deviceId() const;
    void setDeviceId(const QString &deviceId);

    QString name() const;
    void setName(const QString &name);

    QString type() const;
    void setType(const QString &type);

    QString protocol() const;
    void setProtocol(const QString &protocol);

    int connectionStatus() const;
    void setConnectionStatus(int status);

    bool online() const;

    QString room() const;
    void setRoom(const QString &room);

    QString description() const;
    void setDescription(const QString &description);

    QVariantList registers() const;
    void setRegisters(const QVariantList &registers);

    // Connection methods
    Q_INVOKABLE virtual bool connectToDevice() = 0;
    Q_INVOKABLE virtual void disconnectFromDevice() = 0;
    Q_INVOKABLE virtual bool testConnection() = 0;

    // Read/Write operations
    Q_INVOKABLE virtual void readRegister(int address, int functionCode) = 0;
    Q_INVOKABLE virtual void writeRegister(int address, int value, int functionCode) = 0;

    // Serialization
    virtual QJsonObject toJson() const;
    virtual void fromJson(const QJsonObject &json);

signals:
    void deviceIdChanged();
    void nameChanged();
    void typeChanged();
    void protocolChanged();
    void connectionStatusChanged();
    void onlineChanged();
    void roomChanged();
    void descriptionChanged();
    void registersChanged();
    void valueChanged(const QString &registerName, const QVariant &value);
    void errorOccurred(const QString &error);

protected:
    QString m_deviceId;
    QString m_name;
    QString m_type;
    QString m_protocol;
    int m_connectionStatus;
    QString m_room;
    QString m_description;
    QVariantList m_registers;
};

#endif // DEVICE_H
