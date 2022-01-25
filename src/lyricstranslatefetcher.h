#ifndef LYRICSTRANSLATEFETCHER_H
#define LYRICSTRANSLATEFETCHER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "lyricsfetcher.h"

class LyricstranslateFetcher : public QObject, public LyricsFetcher
{
    Q_OBJECT
    Q_INTERFACES(LyricsFetcher)
public:
    explicit LyricstranslateFetcher(QObject *parent = nullptr);
    void fetchLyrics(const QString link) override;
    void fetchList(const QString songTitle) override;
signals:
    void lyricsReady(const QString& lyrics) override;
    void listReady(StringPairList list) override;

private slots:
    void lyricsFetched(QNetworkReply *reply) override;
    void listFetched(QNetworkReply *reply) override;
};

#endif // LYRICSTRANSLATEFETCHER_H
