#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
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

#define VERSION "v1.1.9"

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

private slots:

    void checkConfigFile();
    void songCooked();
    void downloadFinished(int exitCode);
    void showContextMenu(const QPoint &pos);
    void lyricsListFetched(const StringPairList& list);
    void lyricsFetched(const QString& lyrics);
    void downloadProgress(qint64 ist, qint64 max);

    void on_directoryButton_clicked();
    void on_refreshButton_clicked();
    void on_startButton_clicked();
    void on_addSongButton_clicked();
    void on_deleteSongButton_clicked();
    void on_searchOnlineButton_clicked();
    void on_youtubeButton_clicked();
    void on_dropList_textActivated(const QString &arg1);
    void on_tsayCheckBox_stateChanged(int arg1);
    void on_actionUpdate_YTDL_triggered();
    void on_actionUpdate_account_info_triggered();
    void on_actionAbout_triggered();
    void on_actionGuide_triggered();
    void on_actionUpdate_client_triggered();
    void on_actionKey_bindings_triggered();
    void on_actionOptions_triggered();

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
    QFile tracklist;
    QFile dest;
    QTimer* timer;
    QTimer* timeout_timer;
    QTimer* dl_file_timer;
    QMovie *movie;
    QList<LyricsFetcher*> lyrics_fetchers;
    int fetchers_ready;
    int timer_interval;
};

#endif // MAINWINDOW_H
