#ifndef MODBUSRESPONSE_H
#define MODBUSRESPONSE_H

#include <QByteArray>
#include <QVector>

/**
 * @brief Represents a Modbus response frame
 * 
 * This class parses Modbus PDU (Protocol Data Unit) responses.
 */
class ModbusResponse
{
public:
    explicit ModbusResponse();
    
    // Transaction ID (for TCP)
    void setTransactionId(quint16 transactionId);
    quint16 transactionId() const;
    
    // Unit ID
    void setUnitId(quint8 unitId);
    quint8 unitId() const;
    
    // PDU data
    void setPdu(const QByteArray &pdu);
    QByteArray pdu() const;
    
    // Response parsing
    bool isException() const;
    quint8 exceptionCode() const;
    quint8 functionCode() const;
    quint8 byteCount() const;
    
    // Data extraction
    QVector<bool> readCoils() const;
    QVector<quint16> readRegisters() const;
    bool writeSuccess() const;
    
    // Raw data access
    QByteArray rawData() const;

private:
    quint16 m_transactionId;
    quint8 m_unitId;
    QByteArray m_pdu;
};

#endif // MODBUSRESPONSE_H
