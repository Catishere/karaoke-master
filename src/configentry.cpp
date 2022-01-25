#include "configentry.h"

const QList<QPair<QString, QString> > &ConfigEntry::getKeyBindings() const
{
    return keyBindings;
}

void ConfigEntry::setKeyBindings(const QList<QPair<QString, QString> > &newKeyBindings)
{
    keyBindings = newKeyBindings;
}

const QString &ConfigEntry::getPc() const
{
    return pc;
}

void ConfigEntry::setPc(const QString &newPc)
{
    pc = newPc;
}

bool ConfigEntry::getAlwaysDownload() const
{
    return alwaysDownload;
}

void ConfigEntry::setAlwaysDownload(bool newAlwaysDownload)
{
    alwaysDownload = newAlwaysDownload;
}

ConfigEntry::ConfigEntry()
{}

ConfigEntry::ConfigEntry(QString &path)
{
    path.replace("\\","/");
    QStringList pathTokens = path.split("/");

    int gamesFolder = pathTokens.indexOf("common");

    this->name = pathTokens.at(gamesFolder + 2);
    this->full_name = pathTokens.at(gamesFolder + 1);

    while (pathTokens.size() != gamesFolder + 2)
        pathTokens.removeLast();

    QString gamePath = pathTokens.join('/');

    QFile file(gamePath + "/steam_appid.txt");

    QString game_appid;
    if (file.open(QIODevice::ReadOnly))
    {
        game_appid = file.readLine();
        file.close();
    }
    else
        game_appid = "";

    this->code = game_appid.trimmed();
    this->path = path;
    this->status = false;
    this->alwaysDownload = false;
    this->pc = "Average";
}

void ConfigEntry::read(const QJsonObject &json)
{
    name = json["name"].toString();
    path = json["path"].toString();
    code = json["code"].toString();
    full_name = json["full_name"].toString();
    status = json["status"].toBool();

    keyBindings.clear();
    for (auto &key : json["keys"].toObject().keys()) {
        keyBindings.append({key, json["keys"][key].toString()});
    }
    pc = json["pc"].toString();
    alwaysDownload = json["always_download"].toBool();
}

void ConfigEntry::write(QJsonObject &json) const
{
    json["name"] = name;
    json["path"] = path;
    json["code"] = code;
    json["full_name"] = full_name;
    json["status"] = status;
    QJsonObject obj;
    for (auto& key : keyBindings) {
        obj.insert(key.first, key.second);
    }
    json["keys"] = obj;
    json["pc"] = pc;
    json["always_download"] = alwaysDownload;
}

void ConfigEntry::setStatus(bool status)
{
    this->status = status;
}

QString ConfigEntry::getName() const
{
    return name;
}

QString ConfigEntry::getCode() const
{
    return code;
}

QString ConfigEntry::getPath() const
{
    return path;
}

QString ConfigEntry::getFullName() const
{
    return full_name;
}

bool ConfigEntry::getStatus() const
{
    return status;
}


