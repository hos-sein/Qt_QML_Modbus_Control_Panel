#include "ModbusClient.h"

ModbusClient::ModbusClient(QObject *parent)
    : QObject(parent)
    , m_connectionState(ConnectionState::Disconnected)
    , m_unitId(1)
    , m_timeout(1000)
{
}

ModbusClient::~ModbusClient()
{
}
