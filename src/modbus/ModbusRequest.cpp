#include "ModbusRequest.h"

ModbusRequest::ModbusRequest()
    : m_functionCode(0)
    , m_address(0)
    , m_quantity(0)
    , m_value(0)
{
}

void ModbusRequest::setFunctionCode(quint8 functionCode)
{
    m_functionCode = functionCode;
}

quint8 ModbusRequest::functionCode() const
{
    return m_functionCode;
}

void ModbusRequest::setAddress(quint16 address)
{
    m_address = address;
}

quint16 ModbusRequest::address() const
{
    return m_address;
}

void ModbusRequest::setQuantity(quint16 quantity)
{
    m_quantity = quantity;
}

quint16 ModbusRequest::quantity() const
{
    return m_quantity;
}

void ModbusRequest::setValue(quint16 value)
{
    m_value = value;
}

quint16 ModbusRequest::value() const
{
    return m_value;
}

void ModbusRequest::setData(const QByteArray &data)
{
    m_data = data;
}

QByteArray ModbusRequest::data() const
{
    return m_data;
}

QByteArray ModbusRequest::pdu() const
{
    QByteArray pdu;
    
    switch (m_functionCode) {
        case 0x01: // Read Coils
        case 0x02: // Read Discrete Inputs
        case 0x03: // Read Holding Registers
        case 0x04: // Read Input Registers
            pdu.append(static_cast<char>(m_functionCode));
            pdu.append(static_cast<char>((m_address >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_address & 0xFF));
            pdu.append(static_cast<char>((m_quantity >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_quantity & 0xFF));
            break;
            
        case 0x05: // Write Single Coil
            pdu.append(static_cast<char>(m_functionCode));
            pdu.append(static_cast<char>((m_address >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_address & 0xFF));
            pdu.append(static_cast<char>((m_value >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_value & 0xFF));
            break;
            
        case 0x06: // Write Single Register
            pdu.append(static_cast<char>(m_functionCode));
            pdu.append(static_cast<char>((m_address >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_address & 0xFF));
            pdu.append(static_cast<char>((m_value >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_value & 0xFF));
            break;
            
        case 0x0F: // Write Multiple Coils
            pdu.append(static_cast<char>(m_functionCode));
            pdu.append(static_cast<char>((m_address >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_address & 0xFF));
            pdu.append(static_cast<char>((m_quantity >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_quantity & 0xFF));
            pdu.append(static_cast<char>(m_data.size()));
            pdu.append(m_data);
            break;
            
        case 0x10: // Write Multiple Registers
            pdu.append(static_cast<char>(m_functionCode));
            pdu.append(static_cast<char>((m_address >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_address & 0xFF));
            pdu.append(static_cast<char>((m_quantity >> 8) & 0xFF));
            pdu.append(static_cast<char>(m_quantity & 0xFF));
            pdu.append(static_cast<char>(m_data.size()));
            pdu.append(m_data);
            break;
            
        default:
            break;
    }
    
    return pdu;
}

int ModbusRequest::pduSize() const
{
    return pdu().size();
}

ModbusRequest ModbusRequest::readCoils(quint16 address, quint16 quantity)
{
    ModbusRequest request;
    request.setFunctionCode(0x01);
    request.setAddress(address);
    request.setQuantity(quantity);
    return request;
}

ModbusRequest ModbusRequest::readDiscreteInputs(quint16 address, quint16 quantity)
{
    ModbusRequest request;
    request.setFunctionCode(0x02);
    request.setAddress(address);
    request.setQuantity(quantity);
    return request;
}

ModbusRequest ModbusRequest::readHoldingRegisters(quint16 address, quint16 quantity)
{
    ModbusRequest request;
    request.setFunctionCode(0x03);
    request.setAddress(address);
    request.setQuantity(quantity);
    return request;
}

ModbusRequest ModbusRequest::readInputRegisters(quint16 address, quint16 quantity)
{
    ModbusRequest request;
    request.setFunctionCode(0x04);
    request.setAddress(address);
    request.setQuantity(quantity);
    return request;
}

ModbusRequest ModbusRequest::writeSingleCoil(quint16 address, bool value)
{
    ModbusRequest request;
    request.setFunctionCode(0x05);
    request.setAddress(address);
    request.setValue(value ? 0xFF00 : 0x0000);
    return request;
}

ModbusRequest ModbusRequest::writeSingleRegister(quint16 address, quint16 value)
{
    ModbusRequest request;
    request.setFunctionCode(0x06);
    request.setAddress(address);
    request.setValue(value);
    return request;
}

ModbusRequest ModbusRequest::writeMultipleCoils(quint16 address, const QVector<bool> &values)
{
    ModbusRequest request;
    request.setFunctionCode(0x0F);
    request.setAddress(address);
    request.setQuantity(static_cast<quint16>(values.size()));
    
    // Pack boolean values into bytes
    QByteArray data;
    int byteCount = (values.size() + 7) / 8;
    for (int i = 0; i < byteCount; ++i) {
        quint8 byte = 0;
        for (int j = 0; j < 8 && (i * 8 + j) < values.size(); ++j) {
            if (values[i * 8 + j]) {
                byte |= (1 << j);
            }
        }
        data.append(static_cast<char>(byte));
    }
    
    request.setData(data);
    return request;
}

ModbusRequest ModbusRequest::writeMultipleRegisters(quint16 address, const QVector<quint16> &values)
{
    ModbusRequest request;
    request.setFunctionCode(0x10);
    request.setAddress(address);
    request.setQuantity(static_cast<quint16>(values.size()));
    
    // Pack register values into bytes
    QByteArray data;
    for (quint16 value : values) {
        data.append(static_cast<char>((value >> 8) & 0xFF));
        data.append(static_cast<char>(value & 0xFF));
    }
    
    request.setData(data);
    return request;
}
