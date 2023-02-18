#ifndef LYRICSFETCHER_H
#define LYRICSFETCHER_H

#include <QString>
#include <QList>
#include <QPair>
#include <QtPlugin>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include "stringpairlist.h"

class LyricsFetcher
{
public:
    virtual ~LyricsFetcher(){} // do not forget this
    virtual void fetchLyrics(const QString link) = 0;
    virtual void fetchList(const QString songTitle) = 0;

    const QString &getEndpoint() const;
    const QString &getFullName() const;

protected:
    QNetworkAccessManager *manager;
    QString endpoint;
    QString fullName;
    QString id;

signals:
    virtual void lyricsReady(const QString& lyrics) = 0;
    virtual void listReady(StringPairList list) = 0;

protected slots:
    virtual void lyricsFetched(QNetworkReply *reply) = 0;
    virtual void listFetched(QNetworkReply *reply) = 0;
};

inline const QString &LyricsFetcher::getEndpoint() const
{
    return endpoint;
}

inline const QString &LyricsFetcher::getFullName() const
{
    return fullName;
}

Q_DECLARE_INTERFACE(LyricsFetcher, "LyricsFetcher")

#endif // LYRICSFETCHER_H

