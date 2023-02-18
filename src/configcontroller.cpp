#include "configcontroller.h"

ConfigController::ConfigController(QObject *parent)
    : QObject(parent)
{
    loadConfig();
    for (const auto &configEntry : configEntries) {
        if (configEntry.getStatus())
            choose(configEntry.getFullName());
    }
    if (currentConfig.getName().isEmpty() && !configEntries.isEmpty())
        choose(configEntries.first().getFullName());
}

void ConfigController::read(const QJsonObject &json)
{
    configEntries.clear();
    QJsonArray configArray = json["configs"].toArray();
    commonSettings.insert("userdataPath", json["userdata"]);
    commonSettings.insert("updateNotification", json["updateNotification"]);
    if (json.constFind("allowedFetchers") != json.constEnd())
        commonSettings.insert("allowedFetchers", json["allowedFetchers"]);
    else
        commonSettings.insert("allowedFetchers",
                              QJsonObject({
                                 {"Genius", true},
                                 {"Lyrics Translate", true},
                                 {"Musixmatch", true}
                             }));

    for (int configIndex = 0; configIndex < configArray.size(); ++configIndex) {
        QJsonObject configObject = configArray[configIndex].toObject();
        ConfigEntry configEntry;
        configEntry.read(configObject);
        configEntries.append(configEntry);
    }
}

void ConfigController::write(QJsonObject &json) const
{
    QJsonArray configArray;

    foreach (const ConfigEntry configEntry, configEntries)
    {
        QJsonObject configObject;
        configEntry.write(configObject);
        configArray.append(configObject);
    }
    json["configs"] = configArray;
    json["userdata"] = commonSettings["userdataPath"];
    json["updateNotification"] = commonSettings["updateNotification"];
    json["allowedFetchers"] = commonSettings["allowedFetchers"];
}

bool ConfigController::saveConfig()
{
    QFile saveFile(QStringLiteral("config/config.json"));

    saveFile.open(QIODevice::WriteOnly);

    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
    emit listChanged();
    return true;
}

bool ConfigController::loadConfig()
{
    QFile loadFile(QStringLiteral("config/config.json"));

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open save file.");
        return false;
    }

    QByteArray saveData = loadFile.readAll();

    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

    read(loadDoc.object());

    return true;
}


void ConfigController::addConfig(const ConfigEntry &configEntry)
{
    for (auto& entry : configEntries) {
        if (entry.getPath() == configEntry.getPath())
            return;
    }

    configEntries.prepend(configEntry);
    choose(configEntry.getFullName());
}

void ConfigController::removeConfig(int index)
{
    configEntries.removeAt(index);
}

void ConfigController::choose(const QString &full_name)
{

    if (configEntries.isEmpty())
        return;

    for (auto& configEntry : configEntries) {
        if (configEntry.getFullName() == full_name) {
            configEntry.setStatus(true);
            currentConfig = configEntry;
        } else
            configEntry.setStatus(false);
    }

    QString path = currentConfig.getPath();
    QString userpath = QString("/userdata/%1/%2/local/cfg/lyrics_trigger.cfg")
                       .arg(commonSettings["accountId"].toString(),
                            currentConfig.getCode());
    commonSettings.insert("userdataPath",
                          path.left(path.indexOf("/steamapps")) + userpath);
    commonSettings.insert("currentGamePath",
                          path.left(path.indexOf('/',
                                                 path.indexOf("common") + 8)));
    saveConfig();
}

QString ConfigController::getUserDataPath() const
{
    return commonSettings["userdataPath"].toString();
}

QString ConfigController::getCurrentGamePath() const
{
    return commonSettings["currentGamePath"].toString();
}

ConfigEntry* ConfigController::getCurrentConfig()
{
    return &currentConfig;
}

QList<ConfigEntry> ConfigController::getConfigEntries() const
{
    return configEntries;
}

QList<ConfigEntry> &ConfigController::getConfigEntriesRef()
{
    return configEntries;
}

void ConfigController::setAccountId(const QString &accountId)
{
    commonSettings.insert("accountId", accountId);
    choose(currentConfig.getFullName());
}

void ConfigController::setAllowedFetchers(QMap<QString, bool> map)
{
    QJsonObject obj;
    QMapIterator<QString, bool> it(map);
    while (it.hasNext()) {
        it.next();
        obj.insert(it.key(), it.value());
    }
    commonSettings["allowedFetchers"] = obj;
}

QMap<QString, bool> ConfigController::getAllowedFetchers()
{
    QMap<QString, bool> fetchers;
    if (!commonSettings["allowedFetchers"].isObject())
         return fetchers;

    auto obj = commonSettings["allowedFetchers"].toObject();
    for (auto& key : obj.keys()) {
        fetchers.insert(key, obj[key].toBool());
    }
    return fetchers;
}

void ConfigController::setUpdateNotification(bool shown)
{
    commonSettings.insert("updateNotification", shown);
}

bool ConfigController::isUpdateNotification() const
{
    return commonSettings["updateNotification"] == QJsonValue::Undefined
            || !commonSettings["updateNotification"].toBool();
}
