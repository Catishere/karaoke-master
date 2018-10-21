#ifndef CONFIGENTRY_H
#define CONFIGENTRY_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QFile>
#include <QDebug>

class ConfigEntry
{
    QString name;
    QString code;
    QString path;
    QString full_name;
    bool status;

public:

    ConfigEntry();
    ConfigEntry(QString &path);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString getName() const;
    QString getCode() const;
    QString getPath() const;
    QString getFullName() const;
    bool getStatus() const;
    void setStatus(bool status);
};

#endif // CONFIGENTRY_H
