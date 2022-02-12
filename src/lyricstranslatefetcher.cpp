#include "lyricstranslatefetcher.h"

LyricstranslateFetcher::LyricstranslateFetcher(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    endpoint = "https://lyricstranslate.com";
    id = "LT";
    fullName = "Lyrics Translate";
    fetchToken();
}

void LyricstranslateFetcher::fetchLyrics(const QString link)
{
    if (!link.startsWith(endpoint)) return;
    qDebug() << "LT fetching lyrics...";

    QNetworkRequest request;
    request.setUrl(QUrl(link));
    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &LyricstranslateFetcher::lyricsFetched);
    manager->get(request);
}

void LyricstranslateFetcher::fetchList(const QString songTitle)
{
    qDebug() << "LT fetching list...";
    QNetworkRequest req;
    req.setUrl(QUrl("https://cse.google.com/cse/element/v1?"
                    "&hl=en"
                    "&cx=partner-pub-2890000658857916:3199793784"
                    "&q=" + songTitle +
                    "&cse_tok=" + cse_token +
                    "&exp=csqr,cc&callback=google.search.cse.api3773"));

    manager->disconnect();
    connect(manager, &QNetworkAccessManager::finished,
            this, &LyricstranslateFetcher::listFetched);

    manager->get(req);
}

void LyricstranslateFetcher::lyricsFetched(QNetworkReply *reply)
{
    qDebug() << "LT lyrics fetched!";
    QString lyrics;
    QByteArray page = reply->readAll();

    int start = page.indexOf("<div id=\"song-body\"");
    int end = page.indexOf("<div id=\"song-transliteration\"", start);

    lyrics = page.mid(start, end - start);
    QRegularExpression rx("<span class=\"line-number\" style="
                          "\"display: none;\">.+?</span>",
                          QRegularExpression::DotMatchesEverythingOption);
    QRegularExpression regexp("<.+?>",
                              QRegularExpression::DotMatchesEverythingOption);
    lyrics.remove(regexp);
    lyrics.remove(rx);
    lyrics.replace("<div class=\"par\">", "\n");
    QRegularExpression regexp2("\\[.+?\\]");
    lyrics.remove(regexp2);
    lyrics.replace("&quot;", "'");
    lyrics.replace("&#x27;", "'");
    lyrics.replace("&nbsp;", "\n");
    lyrics = lyrics.trimmed();
    emit lyricsReady(lyrics);
    reply->deleteLater();
}

void LyricstranslateFetcher::listFetched(QNetworkReply *reply)
{
    qDebug() << "LT list fetched!";
    StringPairList list;
    QByteArray data = reply->readAll();
    qsizetype start = data.indexOf('{');
    data = data.mid(start, data.size() - start - 2);

    QJsonDocument doc(QJsonDocument::fromJson(data));
    const QJsonValue results = doc["results"];
    const int resultCount = doc["cursor"]["resultCount"].
            toString().
            remove(',').
            toInt();

    if (!results.isNull() && resultCount > 15) {
        for (auto&& result : results.toArray()) {
            auto item = result.toObject().value("titleNoFormatting").toString();
            qsizetype index = item.lastIndexOf(" lyrics +");
            if (index > 0) continue;
            index = item.lastIndexOf(" lyrics");
            if (index > 0) item.truncate(index);
            auto link = result.toObject().value("unescapedUrl").toString();
            item = item.append(id).trimmed();
            list.append({item, link});
        }
    }

    emit listReady(list);
    reply->deleteLater();
}

void LyricstranslateFetcher::fetchToken()
{
    QNetworkRequest req;
    req.setUrl(QUrl("https://www.google.com/cse/cse.js?"
                    "cx=partner-pub-2890000658857916:3199793784"));
    connect(manager, &QNetworkAccessManager::finished,
            this, [=](QNetworkReply *reply) {
        QString page = reply->readAll();
        int start = page.indexOf(TOKEN_KEY) + sizeof(TOKEN_KEY) - 1;
        int end = page.indexOf("\"", start + sizeof(TOKEN_KEY) + 1);
        cse_token = page.mid(start, end - start);
    });
    manager->get(req);
}
