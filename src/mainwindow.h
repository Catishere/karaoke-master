#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QFileDialog>
#include <QDebug>

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

private slots:

    void on_directoryButton_clicked();

    void on_refreshButton_clicked();

    void on_startButton_clicked();

    void on_addSongButton_clicked();

private:
    Ui::MainWindow *ui;
    QString hldir;
    QFile tracklist;
    QFile dest;
};

#endif // MAINWINDOW_H
