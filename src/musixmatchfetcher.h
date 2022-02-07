#ifndef MUSIXMATCHFETCHER_H
#define MUSIXMATCHFETCHER_H

#include <QObject>
#include <QRegularExpression>

#include "lyricsfetcher.h"

#define START_TOKEN "<span class=\"lyrics__content__"
#define END_TOKEN "</span>"

class MusixmatchFetcher : public QObject, public LyricsFetcher
{
    Q_OBJECT
    Q_INTERFACES(LyricsFetcher)
public:
    explicit MusixmatchFetcher(QObject *parent = nullptr);

    // LyricsFetcher interface
public:
    void fetchLyrics(const QString link);
    void fetchList(const QString songTitle);

signals:
    void lyricsReady(const QString &lyrics);
    void listReady(StringPairList list);

protected slots:
    void lyricsFetched(QNetworkReply *reply);
    void listFetched(QNetworkReply *reply);
};

#endif // MUSIXMATCHFETCHER_H
