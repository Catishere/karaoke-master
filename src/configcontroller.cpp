#include "configcontroller.h"

ConfigController::ConfigController()
{
    loadConfig();
    foreach (const ConfigEntry &configEntry, configEntries)
    {
        if (configEntry.getStatus())
            choose(configEntry.getFullName());
    }
}

void ConfigController::read(const QJsonObject &json)
{
    configEntries.clear();
    QJsonArray configArray = json["configs"].toArray();
    userdataPath = json["userdata"].toString();

    for (int configIndex = 0; configIndex < configArray.size(); ++configIndex)
    {
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
    json["userdata"] = userdataPath;
}

bool ConfigController::saveConfig() const
{
    QFile saveFile(QStringLiteral("config/config.json"));

    saveFile.open(QIODevice::WriteOnly);

    QJsonObject gameObject;
    write(gameObject);
    QJsonDocument saveDoc(gameObject);
    saveFile.write(saveDoc.toJson());
    saveFile.close();
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
    configEntries.prepend(configEntry);
    choose(configEntry.getFullName());
    saveConfig();
}

void ConfigController::removeConfig(int index)
{
    configEntries.removeAt(index);
    saveConfig();
}

bool ConfigController::choose(const QString &full_name)
{
    bool ret = false;

    for (int i = 0; i < configEntries.size(); i++)
    {
        ConfigEntry &configEntry = configEntries[i];

        if (configEntry.getStatus())
            configEntry.setStatus(false);

        if (configEntry.getFullName() == full_name)
        {
            currentConfig = &configEntry;
            currentConfig->setStatus(true);
            ret = true;
        }
    }

    QString path = currentConfig->getPath();
    QString userpath = "/userdata/"
            + accountId +"/"
            + currentConfig->getCode()
            + "/local/cfg/lyrics_trigger.cfg";
    userdataPath = path.left(path.indexOf("/steamapps")) + userpath;
    currentGamePath = path.left(path.indexOf('/', path.indexOf("common") + 8));

    saveConfig();

    return ret;
}

QString ConfigController::getUserDataPath() const
{
    return userdataPath;
}

QString ConfigController::getCurrentGamePath() const
{
    return currentGamePath;
}

ConfigEntry ConfigController::getCurrentConfig() const
{
    return *currentConfig;
}

ConfigEntry* ConfigController::getCurrentConfigRef() const
{
    return currentConfig;
}

QList<ConfigEntry> ConfigController::getConfigEntries() const
{
    return configEntries;
}

void ConfigController::setAccountId(const QString &accountId)
{
    this->accountId = accountId;
    choose(currentConfig->getFullName());
}
