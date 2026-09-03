#include "CommandQueue.h"
#include "modbus/ModbusRequest.h"

CommandQueue::CommandQueue(QObject *parent)
    : QObject(parent)
    , m_processing(false)
{
}

void CommandQueue::enqueue(const QString &deviceId, const ModbusRequest &request)
{
    QMutexLocker locker(&m_mutex);
    
    Command cmd;
    cmd.deviceId = deviceId;
    cmd.request = request;
    cmd.retryCount = 0;
    cmd.timestamp = QDateTime::currentMSecsSinceEpoch();
    
    m_queue.enqueue(cmd);
    
    if (!m_processing) {
        emit commandAvailable();
    }
}

Command CommandQueue::dequeue()
{
    QMutexLocker locker(&m_mutex);
    
    if (m_queue.isEmpty()) {
        return Command();
    }
    
    return m_queue.dequeue();
}

bool CommandQueue::isEmpty() const
{
    QMutexLocker locker(&m_mutex);
    return m_queue.isEmpty();
}

int CommandQueue::size() const
{
    QMutexLocker locker(&m_mutex);
    return m_queue.size();
}

void CommandQueue::clear()
{
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
    emit queueEmpty();
}

void CommandQueue::setProcessing(bool processing)
{
    m_processing = processing;
}

bool CommandQueue::isProcessing() const
{
    return m_processing;
}
