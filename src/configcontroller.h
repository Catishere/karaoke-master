#ifndef CONFIGCONTROLLER_H
#define CONFIGCONTROLLER_H

#include <QFile>
#include <QList>
#include <QPair>
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
    void choose(const QString &full_name);

    void addConfig(const ConfigEntry &configEntry);
    void removeConfig(int index);
    ConfigEntry getCurrentConfig() const;
    ConfigEntry* getCurrentConfigRef() const;
    QString getUserDataPath() const;
    QString getCurrentGamePath() const;
    QList<ConfigEntry> getConfigEntries() const;
    void setAccountId(const QString &accountId);
    void setAllowedFetchers(QMap<QString, bool> map);
    QMap<QString, bool> getAllowedFetchers();
    void setUpdateNotification(bool shown);
    bool isUpdateNotification() const;

private:
    ConfigEntry *currentConfig;
    QJsonObject commonSettings;
    QList<ConfigEntry> configEntries;
};

#endif // CONFIGCONTROLLER_H
