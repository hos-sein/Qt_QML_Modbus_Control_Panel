#include "ModbusTcpClient.h"
#include "ModbusRequest.h"
#include "ModbusResponse.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(modbusTcp, "modbus.tcp")

ModbusTcpClient::ModbusTcpClient(QObject *parent)
    : ModbusClient(parent)
    , m_socket(new QTcpSocket(this))
    , m_port(502)
    , m_transactionId(0)
{
    connect(m_socket, &QTcpSocket::connected, this, &ModbusTcpClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &ModbusTcpClient::onDisconnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
            this, &ModbusTcpClient::onError);
    connect(m_socket, &QTcpSocket::readyRead, this, &ModbusTcpClient::onReadyRead);
}

ModbusTcpClient::~ModbusTcpClient()
{
    disconnect();
}

bool ModbusTcpClient::connect()
{
    if (m_connectionState == ConnectionState::Connected) {
        return true;
    }

    m_connectionState = ConnectionState::Connecting;
    emit connectionStateChanged(m_connectionState);

    m_socket->connectToHost(m_ipAddress, m_port);
    
    // Wait for connection with timeout
    if (!m_socket->waitForConnected(m_timeout)) {
        m_errorString = tr("Connection timeout: %1").arg(m_socket->errorString());
        m_connectionState = ConnectionState::Error;
        emit connectionStateChanged(m_connectionState);
        emit errorOccurred(m_errorString);
        return false;
    }

    return true;
}

void ModbusTcpClient::disconnect()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->disconnectFromHost();
        if (m_socket->state() != QAbstractSocket::UnconnectedState) {
            m_socket->waitForDisconnected(1000);
        }
    }
    
    m_connectionState = ConnectionState::Disconnected;
    emit connectionStateChanged(m_connectionState);
    emit disconnected();
}

ModbusClient::ConnectionState ModbusTcpClient::connectionState() const
{
    return m_connectionState;
}

QString ModbusTcpClient::errorString() const
{
    return m_errorString;
}

bool ModbusTcpClient::sendRequest(const ModbusRequest &request)
{
    if (m_connectionState != ConnectionState::Connected) {
        m_errorString = tr("Not connected");
        emit errorOccurred(m_errorString);
        return false;
    }

    // Build MBAP header
    quint16 length = static_cast<quint16>(3 + request.pduSize()); // Unit ID + Function Code + Data
    QByteArray mbapHeader = buildMbapHeader(m_transactionId, length, static_cast<quint8>(m_unitId));
    
    // Build complete frame
    QByteArray frame = mbapHeader + request.pdu();
    
    // Send frame
    qint64 bytesWritten = m_socket->write(frame);
    if (bytesWritten != frame.size()) {
        m_errorString = tr("Failed to write request");
        emit errorOccurred(m_errorString);
        return false;
    }

    qCDebug(modbusTcp) << "TX:" << frame.toHex(' ').toUpper();
    
    return true;
}

void ModbusTcpClient::setUnitId(int unitId)
{
    m_unitId = unitId;
}

int ModbusTcpClient::unitId() const
{
    return m_unitId;
}

void ModbusTcpClient::setTimeout(int timeoutMs)
{
    m_timeout = timeoutMs;
}

int ModbusTcpClient::timeout() const
{
    return m_timeout;
}

void ModbusTcpClient::setIpAddress(const QString &ipAddress)
{
    m_ipAddress = ipAddress;
}

QString ModbusTcpClient::ipAddress() const
{
    return m_ipAddress;
}

void ModbusTcpClient::setPort(int port)
{
    m_port = port;
}

int ModbusTcpClient::port() const
{
    return m_port;
}

void ModbusTcpClient::onConnected()
{
    m_connectionState = ConnectionState::Connected;
    m_errorString.clear();
    emit connectionStateChanged(m_connectionState);
    emit connected();
    emit tcpConnected();
    qCInfo(modbusTcp) << "Connected to" << m_ipAddress << ":" << m_port;
}

void ModbusTcpClient::onDisconnected()
{
    m_connectionState = ConnectionState::Disconnected;
    emit connectionStateChanged(m_connectionState);
    emit disconnected();
    emit tcpDisconnected();
    qCInfo(modbusTcp) << "Disconnected";
}

void ModbusTcpClient::onError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    m_errorString = m_socket->errorString();
    m_connectionState = ConnectionState::Error;
    emit connectionStateChanged(m_connectionState);
    emit errorOccurred(m_errorString);
    qCWarning(modbusTcp) << "Error:" << m_errorString;
}

void ModbusTcpClient::onReadyRead()
{
    m_receiveBuffer.append(m_socket->readAll());
    
    // Process complete frames
    while (m_receiveBuffer.size() >= 8) { // Minimum MBAP header size
        // Parse MBAP header
        quint16 transactionId = static_cast<quint16>((m_receiveBuffer[0] << 8) | m_receiveBuffer[1]);
        quint8 unitId = static_cast<quint8>(m_receiveBuffer[6]);
        quint16 length = static_cast<quint16>((m_receiveBuffer[4] << 8) | m_receiveBuffer[5]);
        
        quint16 frameSize = 6 + length; // MBAP header (6 bytes) + PDU
        
        if (m_receiveBuffer.size() < frameSize) {
            // Wait for more data
            break;
        }
        
        // Extract PDU (Protocol Data Unit)
        QByteArray pdu = m_receiveBuffer.mid(7, length - 1); // Skip unit ID
        
        qCDebug(modbusTcp) << "RX:" << m_receiveBuffer.left(frameSize).toHex(' ').toUpper();
        
        // Create response
        ModbusResponse response;
        response.setTransactionId(transactionId);
        response.setUnitId(unitId);
        response.setPdu(pdu);
        
        emit responseReceived(response);
        
        // Remove processed frame from buffer
        m_receiveBuffer = m_receiveBuffer.mid(frameSize);
    }
}

QByteArray ModbusTcpClient::buildMbapHeader(quint16 transactionId, quint16 length, quint8 unitId)
{
    QByteArray header;
    header.reserve(6);
    header.append(static_cast<char>((transactionId >> 8) & 0xFF));
    header.append(static_cast<char>(transactionId & 0xFF));
    header.append(0x00); // Protocol ID (Modbus)
    header.append(0x00);
    header.append(static_cast<char>((length >> 8) & 0xFF));
    header.append(static_cast<char>(length & 0xFF));
    header.append(static_cast<char>(unitId));
    return header;
}

bool ModbusTcpClient::parseMbapHeader(const QByteArray &data, quint16 &transactionId, quint8 &unitId, quint16 &length)
{
    if (data.size() < 8) {
        return false;
    }
    
    transactionId = static_cast<quint16>((data[0] << 8) | data[1]);
    length = static_cast<quint16>((data[4] << 8) | data[5]);
    unitId = static_cast<quint8>(data[6]);
    
    return true;
}
