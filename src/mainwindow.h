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
#include <QSplashScreen>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

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
    void getLyricsFromURL(const QString &url);
    void handleLyricsReply(const QString &page);
    void downloadSongYoutube(const QString &params);

private slots:

    void on_directoryButton_clicked();

    void on_refreshButton_clicked();

    void on_startButton_clicked();

    void on_addSongButton_clicked();

    void on_deleteSongButton_clicked();

    void managerFinished(QNetworkReply *reply);

    void on_searchOnlineButton_clicked();

    void on_youtubeButton_clicked();

private:
    Ui::MainWindow *ui;
    QString hldir;
    QString temp_lyrics_name;
    QFile tracklist;
    QFile dest;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
};

#endif // MAINWINDOW_H
