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
    QString getUserDataPath() const;
    QString getCurrentGamePath() const;
    QList<ConfigEntry> getConfigEntries() const;

private:
    ConfigEntry *currentConfig;
    QString currentGamePath;
    QString userdataPath;
    QList<ConfigEntry> configEntries;
};

#endif // CONFIGCONTROLLER_H
