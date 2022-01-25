#include "lyricstranslatefetcher.h"

LyricstranslateFetcher::LyricstranslateFetcher(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    endpoint = "https://lyricstranslate.com";
    id = "LT";
}

void LyricstranslateFetcher::fetchLyrics(const QString link)
{
    qDebug() << "LT fetching lyrics...";
    if (!link.startsWith(endpoint)) return;

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
                    "&cse_tok=AJvRUv3WG1mqEr49D9l8jFozQF7N:1643028010926"
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
    if (start < 0) return;
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

    if (!results.isNull()) {
        for (auto&& result : results.toArray()) {
            auto item = result.toObject().value("titleNoFormatting").toString();
            qsizetype index = item.lastIndexOf(" lyrics");
            if (index > 0) item.truncate(index);
            auto link = result.toObject().value("unescapedUrl").toString();
            item = item.append(id).trimmed();
            bool dub = false;
            for (auto& e : list) {
                if (e.first == item)
                   dub = true;
            }
            if (!dub)
                list.append({item, link});
        }
    }

    emit listReady(list);
    reply->deleteLater();
}
