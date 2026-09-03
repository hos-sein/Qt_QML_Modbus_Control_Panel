#ifndef MODBUSRTUCLIENT_H
#define MODBUSRTUCLIENT_H

#include "ModbusClient.h"
#include <QSerialPort>

class ModbusRequest;
class ModbusResponse;

/**
 * @brief Modbus RTU client implementation
 * 
 * Handles Modbus RTU communication using QSerialPort.
 * Implements CRC-16 checksum for data integrity.
 */
class ModbusRtuClient : public ModbusClient
{
    Q_OBJECT

public:
    explicit ModbusRtuClient(QObject *parent = nullptr);
    ~ModbusRtuClient() override;

    // Connection methods
    bool connect() override;
    void disconnect() override;
    ConnectionState connectionState() const override;
    QString errorString() const override;

    // Modbus operations
    bool sendRequest(const ModbusRequest &request) override;

    // Configuration
    void setUnitId(int unitId) override;
    int unitId() const override;
    void setTimeout(int timeoutMs) override;
    int timeout() const override;

    // RTU-specific configuration
    void setPortName(const QString &portName);
    QString portName() const;
    void setBaudRate(int baudRate);
    int baudRate() const;
    void setDataBits(int dataBits);
    int dataBits() const;
    void setParity(QSerialPort::Parity parity);
    QSerialPort::Parity parity() const;
    void setStopBits(QSerialPort::StopBits stopBits);
    QSerialPort::StopBits stopBits() const;

signals:
    void serialConnected();
    void serialDisconnected();

private slots:
    void onReadyRead();
    void onError(QSerialPort::SerialPortError error);

private:
    quint16 calculateCrc(const QByteArray &data);
    bool verifyCrc(const QByteArray &data);

    QSerialPort *m_serialPort;
    QString m_portName;
    int m_baudRate;
    int m_dataBits;
    QSerialPort::Parity m_parity;
    QSerialPort::StopBits m_stopBits;
    QByteArray m_receiveBuffer;
};

#endif // MODBUSRTUCLIENT_H
