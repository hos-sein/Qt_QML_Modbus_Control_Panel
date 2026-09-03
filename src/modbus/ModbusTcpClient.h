#ifndef MODBUSTCPCLIENT_H
#define MODBUSTCPCLIENT_H

#include "ModbusClient.h"
#include <QTcpSocket>
#include <QHostAddress>

class ModbusRequest;
class ModbusResponse;

/**
 * @brief Modbus TCP client implementation
 * 
 * Handles Modbus TCP communication using QTcpSocket.
 * Implements MBAP (Modbus Application Protocol) framing.
 */
class ModbusTcpClient : public ModbusClient
{
    Q_OBJECT

public:
    explicit ModbusTcpClient(QObject *parent = nullptr);
    ~ModbusTcpClient() override;

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

    // TCP-specific configuration
    void setIpAddress(const QString &ipAddress);
    QString ipAddress() const;
    void setPort(int port);
    int port() const;

signals:
    void tcpConnected();
    void tcpDisconnected();

private slots:
    void onConnected();
    void onDisconnected();
    void onError(QAbstractSocket::SocketError socketError);
    void onReadyRead();

private:
    QByteArray buildMbapHeader(quint16 transactionId, quint16 length, quint8 unitId);
    bool parseMbapHeader(const QByteArray &data, quint16 &transactionId, quint8 &unitId, quint16 &length);

    QTcpSocket *m_socket;
    QString m_ipAddress;
    int m_port;
    quint16 m_transactionId;
    QByteArray m_receiveBuffer;
};

#endif // MODBUSTCPCLIENT_H
