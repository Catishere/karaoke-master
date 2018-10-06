#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QFile config_file("config/hldir.conf");

    if (config_file.open(QIODevice::ReadOnly))
    {
        hldir = config_file.readAll();
        config_file.close();
    }

    if (!hldir.isEmpty())
    {
        ui->startButton->setEnabled(true);

        tracklist.setFileName(hldir + "/lyrics_list.cfg");
        dest.setFileName(hldir + "/lyricsmaster.cfg");
    }

    refreshSongList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::refreshSongList()
{
    ui->listWidget->clear();

    QDir dir("lyrics");
    QStringList files = dir.entryList(QDir::Files);

    for (int i = 0; i < files.size(); i++)
    {
        ui->listWidget->addItem(files.at(i));
    }
    return true;
}

bool MainWindow::createTracklistFile()
{
    QString list("echo \"--------------------------------------------------------\"\n");

    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        QString i_str = QString::number(i + 1);
        list.append("echo \"song" + i_str + ": " + ui->listWidget->item(i)->text() + "\"\n");
    }

    list.append("echo \"--------------------------------------------------------\"\n");

    tracklist.write(list.toUtf8());
    return true;
}

bool MainWindow::addSongToConfig(const QString &filename, const QString &id)
{
    QString line;
    QFile source("lyrics/" + filename);

    source.open(QIODevice::ReadOnly);

    int i = 0;

    for (i = 0; !source.atEnd(); i++) {
        line = source.readLine();
        line.remove('"');
        line = line.trimmed();

        if (!line.isEmpty()) {
            QString cfg_line = QString("alias song%1lyrics%2 \"say ~ %3 ;alias spamycs song%4lyrics%5\"\n")
                 .arg(id, QString::number(i), line, id, QString::number(i + 1));
            dest.write(cfg_line.toUtf8());
        }

        else i--;
    }

    QString str = QString("alias song%1lyrics%2 song%3lyrics0;\n")
                        .arg(id, QString::number(i), id);
    dest.write(str.toUtf8());

    return true;
}

bool MainWindow::createSongIndex(const QString &id)
{
    QString str("alias song" + id + " \"alias spamycs song" + id + "lyrics0;\n");
    dest.write(str.toUtf8());
    return true;
}

void MainWindow::on_directoryButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);

    if (!dir.isEmpty() && QDir(dir).entryList().contains("config.cfg"))
    {
        ui->startButton->setEnabled(true);

        QFile cfg("config/hldir.conf");
        cfg.open(QIODevice::WriteOnly);
        cfg.write(dir.toUtf8());
        cfg.close();

        hldir = dir;

        tracklist.setFileName(hldir + "/lyrics_list.cfg");
        dest.setFileName(hldir + "/lyricsmaster.cfg");
    }
}

void MainWindow::on_refreshButton_clicked()
{
    refreshSongList();
}

void MainWindow::on_startButton_clicked()
{
    tracklist.open(QIODevice::WriteOnly | QIODevice::Truncate);
    dest.open(QIODevice::WriteOnly | QIODevice::Truncate);

    ui->status->setText("Working");

    dest.write("alias spamycs say_team \"type exec lyrics_list.cfg in the console to see list with available songs\"\n");
    createTracklistFile();

    for (int i = 0; i < ui->listWidget->count(); i++)
    {
        addSongToConfig(ui->listWidget->item(i)->text(),QString::number(i + 1));
        createSongIndex(QString::number(i+1));
    }

    tracklist.close();
    dest.close();
}

void MainWindow::on_addSongButton_clicked()
{
    ui->err->setText("");

    QStringList songs_list = QFileDialog::getOpenFileNames(this, tr("Open Directory"),
                                                "/", tr("Text Files (*.txt)"));
    for (int i = 0; i < songs_list.size(); i++)
    {
        QString song = songs_list.at(i);
        int pos = song.lastIndexOf(QChar('/'));
        QString song_newpath = "lyrics" + song.right(song.size() - pos);

        QFile song_file(song);
        if (!song_file.copy(song_newpath))
            ui->err->setText(song_file.errorString());
    }
    refreshSongList();
}

void MainWindow::on_deleteSongButton_clicked()
{
    ui->err->setText("");

    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();

    if (items.size() == 0)
        return;

    QString question_content = QString("Are you sure you want to delete %1 song's lyrics?")
            .arg(QString::number(items.size()));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete songs", question_content,
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        for (int i = 0; i < items.size(); i++)
        {
            if (!QFile::remove("lyrics/" + items.at(i)->text()))
                ui->err->setText("Permission error!");
        }
        refreshSongList();
    }
}
