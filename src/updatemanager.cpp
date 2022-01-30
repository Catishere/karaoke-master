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
    this->disconnect(conn);
    QString reply_url = reply->url().toString();

    if (reply_url.startsWith("https://api.github.com")) {
        QByteArray data = reply->readAll();
        QRegularExpression regex("https:..github.com.yt-dlp.yt-dlp.releases.dow"
                                 "nload.\\d{4,4}\\.\\d\\d\\.\\d\\d.yt-dlp.exe");
        QString dl_url = regex.match(data).captured(0);

        QFile ytdlv("ytdl_link.txt");
        if (!ytdlv.open(QIODevice::ReadOnly))
                return;
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

        QNetworkRequest request;
        request.setUrl(QUrl(dl_url));
        QNetworkReply *reply = manager->get(request);
        connect(reply, &QNetworkReply::downloadProgress,
                this, &UpdateManager::downloadProgress);
    } else {
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
