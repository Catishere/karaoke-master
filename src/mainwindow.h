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
#include <QtNetwork/QNetworkReply>

#include "configentry.h"
#include "configcontroller.h"
#include "inputdialog.h"
#include "geniuslyricsfetcher.h"
#include "lyricsfetcher.h"
#include "updatemanager.h"

#define VERSION "v1.0.6"

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

private slots:

    void on_directoryButton_clicked();

    void on_refreshButton_clicked();

    void on_startButton_clicked();

    void on_addSongButton_clicked();

    void on_deleteSongButton_clicked();

    void on_searchOnlineButton_clicked();

    void on_youtubeButton_clicked();

    void checkConfigFile();

    void songCooked();

    void downloadFinished(int exitCode);

    void on_dropList_activated(const QString &item);

    void on_tsayCheckBox_stateChanged(int arg1);

    void on_actionUpdate_YTDL_triggered();

    void on_actionUpdate_account_info_triggered();

    void on_actionAbout_triggered();

    void on_actionGuide_triggered();

    void on_actionUpdate_client_triggered();

    void on_actionKey_bindings_triggered();

    void showContextMenu(const QPoint &pos);

    void on_actionPerformance_triggered();

    void lyricsListFetched(StringPairList list);

    void lyricsFetched(const QString& lyrics);

    void on_actionOptions_triggered();

    void downloadProgress(qint64 ist, qint64 max);

private:
    Ui::MainWindow *ui;
    QString sayType;
    ConfigController configController;
    QString temp_lyrics_name;
    QFile tracklist;
    QFile dest;
    QMovie *movie;
    QTimer* timer;
    QTimer* timeout_timer;
    QTimer* dl_file_timer;
    QString dl_file_name;
    QString search_string;
    UpdateManager *updateManager;
    QList<LyricsFetcher*> lyrics_fetchers;
    int timer_interval;
};

#endif // MAINWINDOW_H
