#include "musixmatchfetcher.h"

MusixmatchFetcher::MusixmatchFetcher(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    endpoint = "https://www.musixmatch.com";
    id = "MM";
}

void MusixmatchFetcher::fetchLyrics(const QString link)
{
    if (!link.startsWith(endpoint)) return;
    qDebug() << "MM fetching lyrics...";

    QNetworkRequest request;
    request.setUrl(QUrl(link));
    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &MusixmatchFetcher::lyricsFetched);
    manager->get(request);
}

void MusixmatchFetcher::fetchList(const QString songTitle)
{
    qDebug() << "MM fetching list...";
    QNetworkRequest req;
    req.setUrl(QUrl("https://www.musixmatch.com/search/" + songTitle));

    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &MusixmatchFetcher::listFetched);

    manager->get(req);
}

void MusixmatchFetcher::lyricsFetched(QNetworkReply *reply)
{
    qDebug() << "MM lyrics fetched!";
    QString lyrics;
    QByteArray page = reply->readAll();

    int start = page.indexOf(START_TOKEN) + sizeof(START_TOKEN) - 1;
    start = page.indexOf(">", start) + 1;
    int end = page.indexOf(END_TOKEN, start);
    lyrics = page.mid(start, end - start) + "\n";

    start = page.indexOf(START_TOKEN, end);
    if (start > 0) {
        start = page.indexOf(">", start + sizeof(START_TOKEN) - 1) + 1;
        end = page.indexOf(END_TOKEN, start);
        lyrics.append(page.mid(start, end - start));
    }

    lyrics.replace("&quot;", "'");
    lyrics.replace("&#x27;", "'");
    lyrics.replace("&nbsp;", "\n");
    lyrics = lyrics.trimmed();
    emit lyricsReady(lyrics);
    reply->deleteLater();
}

void MusixmatchFetcher::listFetched(QNetworkReply *reply)
{
    qDebug() << "MM list fetched!";
    StringPairList list;
    QByteArray page = reply->readAll();
    QString rs("<a class=\"title\" href=\"(?<link>/lyrics/.+?)\"><span>(?<song>"
        ".+?)</span>.+?<a class=\"artist\" href=\"/artist/.+?>(?<artist>.+?)<");

    auto rx = QRegularExpression(rs,
                 QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator i = rx.globalMatch(page);

    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString link = endpoint + match.captured("link");
        QString song = match.captured("song");
        QString artist = match.captured("artist");

        list.append({ QString("%1 - %2%3")
                     .arg(artist, song, id), QString(link)});
    }
    emit listReady(list);
    reply->deleteLater();
}
