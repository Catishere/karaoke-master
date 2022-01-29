#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonArray>

enum class Response {
    UPDATED,
    UP_TO_DATE,
    FAILED
};

class UpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit UpdateManager(QObject *parent = nullptr);
    void updateYTDL();
    void getClientVersion();

signals:
    void YTDLUpdateReady(Response);
    void finished(QString);
    void downloadProgress(qint64, qint64);
private:
    QNetworkAccessManager *manager;
    QMetaObject::Connection conn;

private slots:
    void YTDLLastReleaseFetched(QNetworkReply *reply);

};

#endif // UPDATEMANAGER_H
