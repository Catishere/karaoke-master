#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QFile>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QRegularExpression>

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

signals:
    void YTDLUpdateReady(Response);
    void downloadProgress(qint64, qint64);
private:
    QNetworkAccessManager *manager;

private slots:
    void YTDLLastReleaseFetched(QNetworkReply *reply);

};

#endif // UPDATEMANAGER_H
