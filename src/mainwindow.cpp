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

    manager = new QNetworkAccessManager();
    QObject::connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(managerFinished(QNetworkReply*)));

    refreshSongList();
}

MainWindow::~MainWindow()
{
    delete manager;
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
    if (ui->startButton->text() == "Start")
        ui->startButton->setText("Stop");
    else
    {
        ui->startButton->setText("Start");
    }

    tracklist.open(QIODevice::WriteOnly | QIODevice::Truncate);
    dest.open(QIODevice::WriteOnly | QIODevice::Truncate);

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

void MainWindow::handleLyricsReply(const QString &page)
{
    int start_pos = page.indexOf("<!-- Usage of azlyrics.com content") + 134;
    int end_pos = page.indexOf("</div>", start_pos);

    QString lyrics = page.mid(start_pos, end_pos - start_pos);
    lyrics.replace("<br>", "\n");

    QFile lyrics_file("lyrics/" + temp_lyrics_name + ".txt");

    if (lyrics_file.open(QIODevice::WriteOnly))
    {
        lyrics_file.write(lyrics.toUtf8());
        lyrics_file.close();
    }
    else
        qDebug() << lyrics_file.errorString();

    refreshSongList();

    temp_lyrics_name = "";
}

void MainWindow::getLyricsFromURL(const QString &url)
{
    QNetworkRequest getLyricsReq;
    getLyricsReq.setUrl(QUrl(url));
    QString user_agent = "Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/69.0.3497.100 Safari/537.36";
    getLyricsReq.setHeader(QNetworkRequest::UserAgentHeader, user_agent);
    manager->get(getLyricsReq);
}

void MainWindow::managerFinished(QNetworkReply *reply)
{
    if (reply->error())
    {
        qDebug() << reply->errorString();
        return;
    }

    QString answer = reply->readAll();

    if (reply->url().toString().startsWith("https://www.azlyrics.com/lyrics/"))
    {
        handleLyricsReply(answer);
        return;
    }

    QString regex_str = "\\d{1,2}. <a href=\"(?<link>.+?)\" target=\"_blank\"><b>(?<song>.+?)</b></a>  by <b>(?<artist>.+?)</b><br>";
    QRegularExpression regex = QRegularExpression( regex_str
                               , QRegularExpression::DotMatchesEverythingOption);

    QStringList items;
    QStringList links;

    QRegularExpressionMatchIterator i = regex.globalMatch(answer);

    while (true)
    {
        if (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString link = match.captured("link");
            QString song = match.captured("song");
            QString artist = match.captured("artist");

            items << QString(artist + " - " + song);
            links << QString(link);
        }
        else
            break;
    }

    if (items.size() > 0)
    {
        bool ok;
        QString item = QInputDialog::getItem(this, tr("Choose your song"),
                                                   tr("Choose your song"),
                                                   items, 0, false, &ok);
        if (ok && !item.isEmpty())
        {
            QString lyrics_url = links.at(items.indexOf(item));

            if (temp_lyrics_name.isEmpty())
                temp_lyrics_name = item;
            else
            {
                ui->err->setText("Downloading aborted. Another download in progress.");
                return;
            }
            getLyricsFromURL(lyrics_url);
        }
    }
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

void MainWindow::on_searchOnlineButton_clicked()
{
    bool ok;
       QString text = QInputDialog::getText(this, tr("Online Search"),
                                            tr("Type song name, artist or part of the song"), QLineEdit::Normal,
                                            QDir::home().dirName(), &ok);
       if (ok && !text.isEmpty())
       {
           request.setUrl(QUrl("https://search.azlyrics.com/search.php?q=" + text));
           manager->get(request);
       }
}
