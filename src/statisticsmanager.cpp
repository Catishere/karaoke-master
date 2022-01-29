#include "statisticsmanager.h"

StatisticsManager::StatisticsManager(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);

}

void StatisticsManager::sendLaunch()
{
    QNetworkRequest req;
    QString id = QSysInfo::machineUniqueId();
    req.setUrl(QUrl("https://karaoke-master-stat.herokuapp.com/api/add/" + id));
    manager->get(req);
}
