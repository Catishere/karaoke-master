#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QListWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QRegularExpression>
#include <QMessageBox>
#include <QProcess>
#include <QMovie>
#include <QDebug>
#include <QTimer>

#include "configentry.h"
#include "configcontroller.h"
#include "inputdialog.h"
#include "lyricsfetcher.h"
#include "musixmatchfetcher.h"
#include "geniuslyricsfetcher.h"
#include "lyricstranslatefetcher.h"
#include "updatemanager.h"
#include "statisticsmanager.h"
#include "optionsdialog.h"

#define VERSION "v1.2.5"
#define STEAMAPPS "/Steam/steamapps/common"

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
    void refreshSongList();
    void getPage(const QString &url);
    void handleYTDLUpdateResponse(Response response);
    void downloadSongYoutube(QString &song_name);
    void loadSong(int songid);
    void loadDropListPaths();
    void refreshScriptPaths();
    void updateAccount();
    void updateConfigSongList();
    int getTimerInterval(const QString pc);
    const QStringList getMostRecentUser() const;

private:
    void allLyricsListsFetched();
    void addListWithPriorty(const StringPairList &list);
    void showUpdateNotification();
    void updateAllowedFetchers();
    void connectUi();

private slots:

    void checkConfigFile();
    void songCooked();
    void showContextMenu(const QPoint &pos);
    void lyricsListFetched(const StringPairList& list);
    void lyricsFetched(const QString& lyrics);
    void downloadProgress(qint64 ist, qint64 max);

    void directoryClicked();
    void refreshClicked();
    void startClicked();
    void addSongClicked();
    void deleteSongClicked();
    void searchOnlineClicked();
    void youtubeClicked();
    void dropListChanged(const QString &arg1);
    void tsayChanged(int arg1);
    void updatedYTDLTriggered();
    void updateAccountTriggered();
    void aboutTriggered();
    void guideTriggered();
    void updateClientTriggered();
    void keyBindingsTriggered();
    void optionsTriggered();
    void findGamesTriggered();

private:
    Ui::MainWindow *ui;
    ConfigController configController;
    UpdateManager *updateManager;
    StatisticsManager stats;
    QString sayType;
    QString temp_lyrics_name;
    QString dl_file_name;
    QString search_string;
    StringPairList search_list;
    QList<StringPairList> temp_search_list;
    QFile tracklist;
    QFile dest;
    QTimer* timer;
    QTimer* timeout_timer;
    QMovie *movie;
    QList<LyricsFetcher*> allowed_fetchers;
    QList<LyricsFetcher *> all_fetchers;
    int fetchers_ready;
    int timer_interval;
};

#endif // MAINWINDOW_H
