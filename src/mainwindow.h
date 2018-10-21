#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "configentry.h"
#include "configcontroller.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool createTracklistFile();
    bool addSongToConfig(const QString &filename, const QString &id);
    bool createSongIndex(const QString &id);
    bool refreshSongList();
    void getPage(const QString &url);
    bool handleLyricsReply(QNetworkReply *reply);
    QString getGeniusSongName(const QString &page);
    void handleLyricsSearchReply(QNetworkReply *reply);
    void downloadSongYoutube(const QString &params);
    void loadSong(int songid);
    void loadDropListPaths();

private slots:

    void on_directoryButton_clicked();

    void on_refreshButton_clicked();

    void on_startButton_clicked();

    void on_addSongButton_clicked();

    void on_deleteSongButton_clicked();

    void managerFinished(QNetworkReply *reply);

    void on_searchOnlineButton_clicked();

    void on_youtubeButton_clicked();

    void checkConfigFile();

    void songCooked();

    void on_dropList_activated(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QString hldir;
    QString hldir_root;
    ConfigController configController;
    QString temp_lyrics_name;
    QFile tracklist;
    QFile dest;
    QTimer* timer;
    QTimer* dl_file_timer;
    QString dl_file_name;
    QString search_string;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
};

#endif // MAINWINDOW_H
