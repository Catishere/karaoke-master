#ifndef CONFIGCONTROLLER_H
#define CONFIGCONTROLLER_H

#include <QFile>
#include <QObject>
#include <QList>
#include <QPair>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include "configentry.h"

class ConfigController : public QObject
{
    Q_OBJECT
public:
    explicit ConfigController(QObject *parent = nullptr);

    void read(const QJsonObject &json);
    void write(QJsonObject &json) const;

    bool loadConfig();
    bool saveConfig();
    void choose(const QString &full_name);

    void addConfig(const ConfigEntry &configEntry);
    void removeConfig(int index);
    ConfigEntry* getCurrentConfig();
    QString getUserDataPath() const;
    QString getCurrentGamePath() const;
    QString getCurrentConfigTriggerPath() const;
    QList<ConfigEntry> getConfigEntries() const;
    QList<ConfigEntry> &getConfigEntriesRef();
    void setAccountId(const QString &accountId);
    void setAllowedFetchers(QMap<QString, bool> map);
    QMap<QString, bool> getAllowedFetchers();
    void setUpdateNotification(bool shown);
    bool isUpdateNotification() const;

private:
    ConfigEntry currentConfig;
    QJsonObject commonSettings;
    QList<ConfigEntry> configEntries;

signals:
    void listChanged();
};

#endif // CONFIGCONTROLLER_H
