#ifndef CONFIGENTRY_H
#define CONFIGENTRY_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStringList>
#include <QFile>
#include <QDebug>

typedef QList<QPair<QString, QString>> StringPairList;

class ConfigEntry
{
    QString name;
    QString code;
    QString path;
    QString full_name;
    bool status;
    StringPairList keyBindings;
    QString pc;

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
    const QList<QPair<QString, QString> > &getKeyBindings() const;
    void setKeyBindings(const QList<QPair<QString, QString> > &newKeyBindings);
    const QString &getPc() const;
    void setPc(const QString &newPc);
};

#endif // CONFIGENTRY_H
