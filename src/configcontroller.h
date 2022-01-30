#ifndef CONFIGCONTROLLER_H
#define CONFIGCONTROLLER_H

#include <QFile>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "configentry.h"

class ConfigController
{

public:
    ConfigController();

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    bool loadConfig();
    bool saveConfig() const;
    bool choose(const QString &full_name);

    void addConfig(const ConfigEntry &configEntry);
    void removeConfig(int index);
    ConfigEntry getCurrentConfig() const;
    ConfigEntry* getCurrentConfigRef() const;
    QString getUserDataPath() const;
    QString getCurrentGamePath() const;
    QList<ConfigEntry> getConfigEntries() const;
    void setAccountId(const QString &accountId);
    void setUpdateNotification();
    bool isUpdateNotification() const;

private:
    ConfigEntry *currentConfig;
    QJsonObject commonSettings;
    QList<ConfigEntry> configEntries;
};

#endif // CONFIGCONTROLLER_H
