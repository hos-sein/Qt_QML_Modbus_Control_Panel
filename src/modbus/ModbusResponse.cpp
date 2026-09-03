#include "ModbusResponse.h"

ModbusResponse::ModbusResponse()
    : m_transactionId(0)
    , m_unitId(0)
{
}

void ModbusResponse::setTransactionId(quint16 transactionId)
{
    m_transactionId = transactionId;
}

quint16 ModbusResponse::transactionId() const
{
    return m_transactionId;
}

void ModbusResponse::setUnitId(quint8 unitId)
{
    m_unitId = unitId;
}

quint8 ModbusResponse::unitId() const
{
    return m_unitId;
}

void ModbusResponse::setPdu(const QByteArray &pdu)
{
    m_pdu = pdu;
}

QByteArray ModbusResponse::pdu() const
{
    return m_pdu;
}

bool ModbusResponse::isException() const
{
    if (m_pdu.isEmpty()) {
        return false;
    }
    return (m_pdu[0] & 0x80) != 0;
}

quint8 ModbusResponse::exceptionCode() const
{
    if (m_pdu.size() < 2 || !isException()) {
        return 0;
    }
    return static_cast<quint8>(m_pdu[1]);
}

quint8 ModbusResponse::functionCode() const
{
    if (m_pdu.isEmpty()) {
        return 0;
    }
    return static_cast<quint8>(m_pdu[0] & 0x7F);
}

quint8 ModbusResponse::byteCount() const
{
    if (m_pdu.size() < 2) {
        return 0;
    }
    return static_cast<quint8>(m_pdu[1]);
}

QVector<bool> ModbusResponse::readCoils() const
{
    QVector<bool> coils;
    
    if (m_pdu.size() < 2) {
        return coils;
    }
    
    quint8 byteCount = static_cast<quint8>(m_pdu[1]);
    
    for (int i = 0; i < byteCount && (2 + i) < m_pdu.size(); ++i) {
        quint8 byte = static_cast<quint8>(m_pdu[2 + i]);
        for (int j = 0; j < 8; ++j) {
            coils.append((byte & (1 << j)) != 0);
        }
    }
    
    return coils;
}

QVector<quint16> ModbusResponse::readRegisters() const
{
    QVector<quint16> registers;
    
    if (m_pdu.size() < 2) {
        return registers;
    }
    
    quint8 byteCount = static_cast<quint8>(m_pdu[1]);
    
    for (int i = 0; i < byteCount / 2 && (2 + i * 2 + 1) < m_pdu.size(); ++i) {
        quint16 value = static_cast<quint16>(
            (static_cast<quint8>(m_pdu[2 + i * 2]) << 8) |
            static_cast<quint8>(m_pdu[2 + i * 2 + 1])
        );
        registers.append(value);
    }
    
    return registers;
}

bool ModbusResponse::writeSuccess() const
{
    if (m_pdu.size() < 5) {
        return false;
    }
    
    // For write single coil/register and write multiple coils/registers,
    // the response echoes the request
    quint8 fc = functionCode();
    if (fc == 0x05 || fc == 0x06 || fc == 0x0F || fc == 0x10) {
        return true;
    }
    
    return false;
}

QByteArray ModbusResponse::rawData() const
{
    return m_pdu;
}
