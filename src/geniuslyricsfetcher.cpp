#include "geniuslyricsfetcher.h"

GeniusLyricsFetcher::GeniusLyricsFetcher(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    endpoint = "https://genius.com";
    id = "GE";
}

void GeniusLyricsFetcher::fetchLyrics(QString link)
{
    if (!link.startsWith(endpoint)) return;
    qDebug() << "Genius fetching lyrics...";

    QNetworkRequest request;
    request.setUrl(QUrl(link));
    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &GeniusLyricsFetcher::lyricsFetched);
    manager->get(request);
}

void GeniusLyricsFetcher::fetchList(const QString songTitle)
{
    qDebug() << "Genius fetching list...";
    QNetworkRequest request;
    request.setUrl(QUrl("https://genius.com/api/search/song?page=1&q="
                        + songTitle));
    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &GeniusLyricsFetcher::listFetched);
    manager->get(request);
}

void GeniusLyricsFetcher::lyricsFetched(QNetworkReply *reply)
{
    qDebug() << "Genius lyrics fetched!";
    QString lyrics;
    QByteArray page = reply->readAll();

    int start = page.indexOf("<div data-lyrics-container=\"true\"");
    int end = page.indexOf("</div>", start);
    int temp = page.indexOf("<div data-lyrics-container=\"true\"");
    while (temp >= 0){
        end = page.indexOf("</div>", temp);
        temp = page.indexOf("<div data-lyrics-container=\"true\"", temp + 10);
    }


    lyrics = page.mid(start, end - start);
    lyrics.replace("<br/>", "\n");
    QRegularExpression regexp("<.+?>",
                              QRegularExpression::DotMatchesEverythingOption);
    lyrics = lyrics.remove(regexp);
    QRegularExpression regexp2("\\[.+?\\]");
    lyrics.remove(regexp2);
    lyrics.replace("&quot;", "'");
    lyrics.replace("&#x27;", "'");
    lyrics = lyrics.trimmed();
    emit lyricsReady(lyrics);
    reply->deleteLater();
}

void GeniusLyricsFetcher::listFetched(QNetworkReply *reply)
{
    qDebug() << "Genius list fetched!";
    QString regex_str;
    QString contentbox;
    StringPairList list;
    contentbox = "Choose song from genius.com";
    regex_str = "\"full_title\":\"(?<song>.+?) by.(?<artist>.+?)\".+?\"path\":\"(?<link>.+?)\"";

    QRegularExpression regex = QRegularExpression( regex_str,
                                                   QRegularExpression::DotMatchesEverythingOption);
    QByteArray page = reply->readAll();

    QRegularExpressionMatchIterator i = regex.globalMatch(page);

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
