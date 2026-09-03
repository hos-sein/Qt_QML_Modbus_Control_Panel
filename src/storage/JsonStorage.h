#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include <QObject>
#include <QString>
#include <QVariantMap>

class JsonStorage : public QObject
{
    Q_OBJECT
public:
    explicit JsonStorage(QObject *parent = nullptr);
    
    Q_INVOKABLE bool load(const QString &filePath);
    Q_INVOKABLE bool save(const QString &filePath) const;
    
    Q_INVOKABLE QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);
    Q_INVOKABLE bool contains(const QString &key) const;
    Q_INVOKABLE void remove(const QString &key);
    Q_INVOKABLE void clear();
    
    Q_INVOKABLE QVariantMap getAll() const;

signals:
    void dataChanged();

private:
    QVariantMap m_data;
};

#endif // JSONSTORAGE_H
