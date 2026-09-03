#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include <QObject>
#include <QHostAddress>

class ModbusRequest;
class ModbusResponse;

/**
 * @brief Base class for all Modbus clients (TCP and RTU)
 * 
 * This abstract class defines the common interface for Modbus communication.
 * Concrete implementations (ModbusTcpClient, ModbusRtuClient) inherit from this.
 */
class ModbusClient : public QObject
{
    Q_OBJECT

public:
    explicit ModbusClient(QObject *parent = nullptr);
    virtual ~ModbusClient();

    // Connection state
    enum class ConnectionState {
        Disconnected,
        Connecting,
        Connected,
        Error
    };
    Q_ENUM(ConnectionState)

    // Modbus function codes
    enum class FunctionCode {
        ReadCoils = 0x01,
        ReadDiscreteInputs = 0x02,
        ReadHoldingRegisters = 0x03,
        ReadInputRegisters = 0x04,
        WriteSingleCoil = 0x05,
        WriteSingleRegister = 0x06,
        WriteMultipleCoils = 0x0F,
        WriteMultipleRegisters = 0x10
    };
    Q_ENUM(FunctionCode)

    // Data types for register interpretation
    enum class DataType {
        Bool,
        UInt16,
        Int16,
        UInt32,
        Int32,
        Float32
    };
    Q_ENUM(DataType)

    // Virtual methods that must be implemented by concrete classes
    virtual bool connect() = 0;
    virtual void disconnect() = 0;
    virtual ConnectionState connectionState() const = 0;
    virtual QString errorString() const = 0;

    // Modbus operations
    virtual bool sendRequest(const ModbusRequest &request) = 0;
    
    // Configuration
    virtual void setUnitId(int unitId) = 0;
    virtual int unitId() const = 0;
    virtual void setTimeout(int timeoutMs) = 0;
    virtual int timeout() const = 0;

signals:
    void connected();
    void disconnected();
    void connectionStateChanged(ModbusClient::ConnectionState state);
    void responseReceived(const ModbusResponse &response);
    void errorOccurred(const QString &error);

protected:
    ConnectionState m_connectionState;
    int m_unitId;
    int m_timeout;
    QString m_errorString;
};

#endif // MODBUSCLIENT_H
