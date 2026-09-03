#ifndef COMMANDQUEUE_H
#define COMMANDQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>

class ModbusClient;
class ModbusRequest;

struct Command {
    QString deviceId;
    ModbusRequest request;
    int retryCount;
    qint64 timestamp;
};

class CommandQueue : public QObject
{
    Q_OBJECT
public:
    explicit CommandQueue(QObject *parent = nullptr);
    
    void enqueue(const QString &deviceId, const ModbusRequest &request);
    Command dequeue();
    bool isEmpty() const;
    int size() const;
    void clear();
    
    void setProcessing(bool processing);
    bool isProcessing() const;

signals:
    void commandAvailable();
    void queueEmpty();

private:
    QQueue<Command> m_queue;
    mutable QMutex m_mutex;
    bool m_processing;
};

#endif // COMMANDQUEUE_H
