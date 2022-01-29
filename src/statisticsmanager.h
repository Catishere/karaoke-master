#ifndef STATISTICSMANAGER_H
#define STATISTICSMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

class StatisticsManager : public QObject
{
    Q_OBJECT

    QNetworkAccessManager *manager;

public:
    explicit StatisticsManager(QObject *parent = nullptr);
    void sendLaunch();

signals:

};

#endif // STATISTICSMANAGER_H
