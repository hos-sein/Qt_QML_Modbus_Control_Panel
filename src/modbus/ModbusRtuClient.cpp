#include "ModbusRtuClient.h"
#include "ModbusRequest.h"
#include "ModbusResponse.h"
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(modbusRtu, "modbus.rtu")

ModbusRtuClient::ModbusRtuClient(QObject *parent)
    : ModbusClient(parent)
    , m_serialPort(new QSerialPort(this))
    , m_portName("")
    , m_baudRate(9600)
    , m_dataBits(8)
    , m_parity(QSerialPort::NoParity)
    , m_stopBits(QSerialPort::OneStop)
{
    connect(m_serialPort, &QSerialPort::readyRead, this, &ModbusRtuClient::onReadyRead);
    connect(m_serialPort, QOverload<QSerialPort::SerialPortError>::of(&QSerialPort::errorOccurred),
            this, &ModbusRtuClient::onError);
}

ModbusRtuClient::~ModbusRtuClient()
{
    disconnect();
}

bool ModbusRtuClient::connect()
{
    if (m_connectionState == ConnectionState::Connected) {
        return true;
    }

    m_connectionState = ConnectionState::Connecting;
    emit connectionStateChanged(m_connectionState);

    m_serialPort->setPortName(m_portName);
    m_serialPort->setBaudRate(m_baudRate);
    m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(m_dataBits));
    m_serialPort->setParity(m_parity);
    m_serialPort->setStopBits(m_stopBits);

    if (!m_serialPort->open(QIODevice::ReadWrite)) {
        m_errorString = tr("Failed to open serial port: %1").arg(m_serialPort->errorString());
        m_connectionState = ConnectionState::Error;
        emit connectionStateChanged(m_connectionState);
        emit errorOccurred(m_errorString);
        return false;
    }

    m_connectionState = ConnectionState::Connected;
    emit connectionStateChanged(m_connectionState);
    emit connected();
    emit serialConnected();
    
    qCInfo(modbusRtu) << "Connected to" << m_portName << "@" << m_baudRate;
    return true;
}

void ModbusRtuClient::disconnect()
{
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
    
    m_connectionState = ConnectionState::Disconnected;
    emit connectionStateChanged(m_connectionState);
    emit disconnected();
    emit serialDisconnected();
    
    qCInfo(modbusRtu) << "Disconnected";
}

ModbusClient::ConnectionState ModbusRtuClient::connectionState() const
{
    return m_connectionState;
}

QString ModbusRtuClient::errorString() const
{
    return m_errorString;
}

bool ModbusRtuClient::sendRequest(const ModbusRequest &request)
{
    if (m_connectionState != ConnectionState::Connected) {
        m_errorString = tr("Not connected");
        emit errorOccurred(m_errorString);
        return false;
    }

    // Build RTU frame (Unit ID + PDU + CRC)
    QByteArray frame;
    frame.append(static_cast<char>(m_unitId));
    frame.append(request.pdu());
    
    // Calculate and append CRC
    quint16 crc = calculateCrc(frame);
    frame.append(static_cast<char>(crc & 0xFF));
    frame.append(static_cast<char>((crc >> 8) & 0xFF));
    
    // Send frame
    qint64 bytesWritten = m_serialPort->write(frame);
    if (bytesWritten != frame.size()) {
        m_errorString = tr("Failed to write request");
        emit errorOccurred(m_errorString);
        return false;
    }

    // Flush output buffer
    m_serialPort->flush();

    qCDebug(modbusRtu) << "TX:" << frame.toHex(' ').toUpper();
    
    return true;
}

void ModbusRtuClient::setUnitId(int unitId)
{
    m_unitId = unitId;
}

int ModbusRtuClient::unitId() const
{
    return m_unitId;
}

void ModbusRtuClient::setTimeout(int timeoutMs)
{
    m_timeout = timeoutMs;
}

int ModbusRtuClient::timeout() const
{
    return m_timeout;
}

void ModbusRtuClient::setPortName(const QString &portName)
{
    m_portName = portName;
}

QString ModbusRtuClient::portName() const
{
    return m_portName;
}

void ModbusRtuClient::setBaudRate(int baudRate)
{
    m_baudRate = baudRate;
}

int ModbusRtuClient::baudRate() const
{
    return m_baudRate;
}

void ModbusRtuClient::setDataBits(int dataBits)
{
    m_dataBits = dataBits;
}

int ModbusRtuClient::dataBits() const
{
    return m_dataBits;
}

void ModbusRtuClient::setParity(QSerialPort::Parity parity)
{
    m_parity = parity;
}

QSerialPort::Parity ModbusRtuClient::parity() const
{
    return m_parity;
}

void ModbusRtuClient::setStopBits(QSerialPort::StopBits stopBits)
{
    m_stopBits = stopBits;
}

QSerialPort::StopBits ModbusRtuClient::stopBits() const
{
    return m_stopBits;
}

void ModbusRtuClient::onReadyRead()
{
    m_receiveBuffer.append(m_serialPort->readAll());
    
    // Process complete frames (minimum: Unit ID + Function Code + Data + CRC = 4 bytes)
    while (m_receiveBuffer.size() >= 4) {
        // Try to find a valid frame
        bool frameFound = false;
        
        for (int i = 0; i <= m_receiveBuffer.size() - 4; ++i) {
            // Check if we have enough data for a complete frame
            int frameLength = 0;
            
            if (m_receiveBuffer.size() >= i + 4) {
                // Estimate frame length based on function code
                quint8 functionCode = static_cast<quint8>(m_receiveBuffer[i + 1]);
                
                if (functionCode & 0x80) {
                    // Exception response
                    frameLength = 5; // Unit ID + Function Code + Exception Code + CRC
                } else {
                    // Normal response - estimate based on function code
                    switch (functionCode) {
                        case 0x01: // Read Coils
                        case 0x02: // Read Discrete Inputs
                        case 0x03: // Read Holding Registers
                        case 0x04: // Read Input Registers
                            if (m_receiveBuffer.size() >= i + 5) {
                                quint8 byteCount = static_cast<quint8>(m_receiveBuffer[i + 2]);
                                frameLength = 4 + byteCount + 2; // Unit ID + Function Code + Byte Count + Data + CRC
                            }
                            break;
                        case 0x05: // Write Single Coil
                        case 0x06: // Write Single Register
                            frameLength = 8; // Unit ID + Function Code + Address + Value + CRC
                            break;
                        case 0x0F: // Write Multiple Coils
                        case 0x10: // Write Multiple Registers
                            if (m_receiveBuffer.size() >= i + 7) {
                                frameLength = 9; // Unit ID + Function Code + Address + Quantity + CRC
                            }
                            break;
                        default:
                            frameLength = 4; // Minimum frame
                            break;
                    }
                }
                
                if (frameLength > 0 && m_receiveBuffer.size() >= i + frameLength) {
                    // Extract frame
                    QByteArray frame = m_receiveBuffer.mid(i, frameLength);
                    
                    // Verify CRC
                    if (verifyCrc(frame)) {
                        // Extract PDU (skip Unit ID and CRC)
                        QByteArray pdu = frame.mid(1, frameLength - 3);
                        
                        qCDebug(modbusRtu) << "RX:" << frame.toHex(' ').toUpper();
                        
                        // Create response
                        ModbusResponse response;
                        response.setUnitId(static_cast<quint8>(m_receiveBuffer[i]));
                        response.setPdu(pdu);
                        
                        emit responseReceived(response);
                        
                        // Remove processed frame from buffer
                        m_receiveBuffer = m_receiveBuffer.mid(i + frameLength);
                        frameFound = true;
                        break;
                    } else {
                        qCWarning(modbusRtu) << "CRC error in received frame";
                    }
                }
            }
        }
        
        if (!frameFound) {
            // No valid frame found, wait for more data
            break;
        }
    }
}

void ModbusRtuClient::onError(QSerialPort::SerialPortError error)
{
    Q_UNUSED(error);
    m_errorString = m_serialPort->errorString();
    m_connectionState = ConnectionState::Error;
    emit connectionStateChanged(m_connectionState);
    emit errorOccurred(m_errorString);
    qCWarning(modbusRtu) << "Error:" << m_errorString;
}

quint16 ModbusRtuClient::calculateCrc(const QByteArray &data)
{
    quint16 crc = 0xFFFF;
    
    for (int pos = 0; pos < data.size(); ++pos) {
        crc ^= static_cast<quint8>(data[pos]);
        
        for (int i = 8; i != 0; --i) {
            if ((crc & 0x0001) != 0) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

bool ModbusRtuClient::verifyCrc(const QByteArray &data)
{
    if (data.size() < 3) {
        return false;
    }
    
    // Extract received CRC (last 2 bytes, little-endian)
    quint16 receivedCrc = static_cast<quint16>(
        (static_cast<quint8>(data[data.size() - 1]) << 8) |
        static_cast<quint8>(data[data.size() - 2])
    );
    
    // Calculate CRC of data without the last 2 bytes
    quint16 calculatedCrc = calculateCrc(data.left(data.size() - 2));
    
    return receivedCrc == calculatedCrc;
}
