#ifndef GENIUSLYRICSFETCHER_H
#define GENIUSLYRICSFETCHER_H

#include <QObject>
#include "lyricsfetcher.h"

class GeniusLyricsFetcher : public QObject, public LyricsFetcher
{
    Q_OBJECT
    Q_INTERFACES(LyricsFetcher)
public:
    explicit GeniusLyricsFetcher(QObject *parent = nullptr);
    void fetchLyrics(const QString link) override;
    void fetchList(const QString songTitle) override;
signals:
    void lyricsReady(const QString& lyrics) override;
    void listReady(StringPairList list) override;

private slots:
    void lyricsFetched(QNetworkReply *reply) override;
    void listFetched(QNetworkReply *reply) override;
};

#endif // GENIUSLYRICSFETCHER_H
