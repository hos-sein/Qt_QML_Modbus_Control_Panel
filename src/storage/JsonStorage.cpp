#include "JsonStorage.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

JsonStorage::JsonStorage(QObject *parent)
    : QObject(parent)
{
}

bool JsonStorage::load(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isObject()) {
        m_data = doc.object().toVariantMap();
        emit dataChanged();
        return true;
    }

    return false;
}

bool JsonStorage::save(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QJsonObject obj = QJsonObject::fromVariantMap(m_data);
    QJsonDocument doc(obj);

    file.write(doc.toJson());
    file.close();

    return true;
}

QVariant JsonStorage::getValue(const QString &key, const QVariant &defaultValue) const
{
    return m_data.value(key, defaultValue);
}

void JsonStorage::setValue(const QString &key, const QVariant &value)
{
    m_data[key] = value;
    emit dataChanged();
}

bool JsonStorage::contains(const QString &key) const
{
    return m_data.contains(key);
}

void JsonStorage::remove(const QString &key)
{
    m_data.remove(key);
    emit dataChanged();
}

void JsonStorage::clear()
{
    m_data.clear();
    emit dataChanged();
}

QVariantMap JsonStorage::getAll() const
{
    return m_data;
}
