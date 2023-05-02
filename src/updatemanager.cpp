#include "updatemanager.h"

UpdateManager::UpdateManager(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void UpdateManager::updateYTDL()
{
    QNetworkRequest req;
    req.setUrl(QUrl("https://api.github.com/repos/yt-dlp/"
                    "yt-dlp/releases/latest"));
    this->disconnect(conn);
    conn = connect(manager, &QNetworkAccessManager::finished,
            this, &UpdateManager::YTDLLastReleaseFetched);
    manager->get(req);
}

void UpdateManager::getUpdateInfo()
{
    QNetworkRequest req;
    req.setUrl(QUrl("https://api.github.com/repos/Catishere/"
                    "karaoke-master/releases/latest"));

    conn = connect(manager, &QNetworkAccessManager::finished,
            this, [this](QNetworkReply *reply) {
        emit finished(reply->readAll());
        this->disconnect(conn);
    });

    manager->get(req);
}

void UpdateManager::YTDLLastReleaseFetched(QNetworkReply *reply)
{
    QString reply_url = reply->url().toString();

    if (reply_url.startsWith("https://api.github.com")) {
        QByteArray data = reply->readAll();
        QRegularExpression regex("https:..github.com.yt-dlp.yt-dlp.releases"
                                 ".download.+?yt-dlp.exe");
        QString dl_url = regex.match(data).captured(0);

        QFile ytdlv("ytdl_link.txt");

        if (!ytdlv.open(QIODevice::ReadWrite)) {
            qDebug() << "Failed to open file for reading/writing";
            emit YTDLUpdateReady(Response::FAILED);
            return;
        }

        QString saved_url = ytdlv.readAll();
        ytdlv.close();

        if (dl_url == saved_url) {
            emit YTDLUpdateReady(Response::UP_TO_DATE);
            return;
        }

        if (!ytdlv.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open file for writing";
            emit YTDLUpdateReady(Response::FAILED);
            return;
        }

        ytdlv.write(dl_url.toLatin1());
        ytdlv.close();

        // Hardcoded fix for broken build 2022.02.03
        if (dl_url.endsWith("2022.02.03/yt-dlp.exe"))
            dl_url = "https://github.com/yt-dlp/yt-dlp/releases/download/"
                     "2022.01.21/yt-dlp.exe";

        QNetworkRequest request;
        request.setUrl(QUrl(dl_url));
        QNetworkReply *reply = manager->get(request);
        connect(reply, &QNetworkReply::downloadProgress,
                this, &UpdateManager::downloadProgress);
    } else {
        this->disconnect(conn);
        QFile file("yt-dlp.exe");
        if (!file.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open yt-dlp.exe for writing";
            emit YTDLUpdateReady(Response::FAILED);
            return;
        }
        file.write(reply->readAll());
        file.close();
        emit YTDLUpdateReady(Response::UPDATED);
    }

    reply->deleteLater();
}
