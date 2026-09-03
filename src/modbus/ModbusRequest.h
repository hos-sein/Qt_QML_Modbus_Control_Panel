#ifndef MODBUSREQUEST_H
#define MODBUSREQUEST_H

#include <QByteArray>
#include <QObject>

/**
 * @brief Represents a Modbus request frame
 * 
 * This class builds Modbus PDU (Protocol Data Unit) for various function codes.
 */
class ModbusRequest
{
public:
    explicit ModbusRequest();
    
    // Function code
    void setFunctionCode(quint8 functionCode);
    quint8 functionCode() const;
    
    // Address and quantity for read/write operations
    void setAddress(quint16 address);
    quint16 address() const;
    void setQuantity(quint16 quantity);
    quint16 quantity() const;
    
    // Value for single write operations
    void setValue(quint16 value);
    quint16 value() const;
    
    // Data for multiple write operations
    void setData(const QByteArray &data);
    QByteArray data() const;
    
    // Build the PDU
    QByteArray pdu() const;
    int pduSize() const;
    
    // Factory methods for common operations
    static ModbusRequest readCoils(quint16 address, quint16 quantity);
    static ModbusRequest readDiscreteInputs(quint16 address, quint16 quantity);
    static ModbusRequest readHoldingRegisters(quint16 address, quint16 quantity);
    static ModbusRequest readInputRegisters(quint16 address, quint16 quantity);
    static ModbusRequest writeSingleCoil(quint16 address, bool value);
    static ModbusRequest writeSingleRegister(quint16 address, quint16 value);
    static ModbusRequest writeMultipleCoils(quint16 address, const QVector<bool> &values);
    static ModbusRequest writeMultipleRegisters(quint16 address, const QVector<quint16> &values);

private:
    quint8 m_functionCode;
    quint16 m_address;
    quint16 m_quantity;
    quint16 m_value;
    QByteArray m_data;
};

#endif // MODBUSREQUEST_H
