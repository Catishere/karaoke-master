#ifndef CONFIGENTRY_H
#define CONFIGENTRY_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QMap>
#include <QFile>
#include <QDebug>

#include "stringpairlist.h"

class ConfigEntry
{
    QString name;
    QString code;
    QString path;
    QString full_name;
    bool status;
    StringPairList keyBindings;
    QString pc;
    bool alwaysDownload;

public:

    ConfigEntry();
    ConfigEntry(QString path);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    QString getName() const;
    QString getCode() const;
    QString getPath() const;
    QString getFullName() const;
    bool getStatus() const;
    void setStatus(bool status);
    const StringPairList &getKeyBindings() const;
    void setKeyBindings(const StringPairList &newKeyBindings);
    const QString &getPc() const;
    void setPc(const QString &newPc);
    bool getAlwaysDownload() const;
    void setAlwaysDownload(bool newAlwaysDownload);
};

#endif // CONFIGENTRY_H
