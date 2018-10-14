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
        karaokePathsConf(config_file.readAll());
        config_file.close();
    }

    ui->tableWidget->setColumnWidth(0, 40);
    ui->tableWidget->setColumnWidth(1, 40);

    if (!hldir.isEmpty())
    {
        ui->startButton->setEnabled(true);
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

void MainWindow::karaokePathsConf(QString hlpath)
{
    if (!hlpath.isEmpty())
    {

        hldir = hlpath.replace("\\","/");
        hldir_root = hldir.left(hldir.indexOf("/csgo")).replace("\\","/");

        userdatapath = hldir.left(hldir.indexOf("/steamapps"));
        userdatapath.append("/userdata/240818586/730/local/cfg/lyrics_trigger.cfg");
        tracklist.setFileName(hldir + "/lyrics_list.cfg");
        dest.setFileName(hldir + "/lyricsmaster.cfg");
    }
}

bool MainWindow::refreshSongList()
{
    ui->tableWidget->clearContents();

    QDir dir("lyrics");
    QStringList lyrics = dir.entryList(QDir::Files);

    QDir song_dir("songs");
    QStringList songs = song_dir.entryList(QDir::Files);

    ui->tableWidget->setRowCount(lyrics.size());

    for (int i = 0; i < lyrics.size(); i++)
    {
        QString name = lyrics.at(i);

        if (!name.endsWith("txt"))
            continue;

        name = name.left(name.size() - 4);

        QTableWidgetItem *lyrics_checkbox = new QTableWidgetItem("Yes");
        QTableWidgetItem *song_checkbox = new QTableWidgetItem("No");
        ui->tableWidget->setItem(i, 1, lyrics_checkbox);

        if (QFileInfo::exists("songs/" + name + ".wav"))
        {
            song_checkbox->setText("Yes");
        }
        else
        {
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() - 1);
        }

        ui->tableWidget->setItem(i, 0, song_checkbox);
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(name));
    }

    int song_pos = lyrics.size();

    for (int i = 0; i < songs.size(); i++)
    {
        QString name = songs.at(i);

        if (!name.endsWith(".wav"))
        {
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() - 1);
            song_pos--;
            continue;
        }
        name = name.left(name.size() - 4);

        if (!QFileInfo::exists("lyrics/" + name + ".txt"))
        {
            QTableWidgetItem *lyrics_checkbox = new QTableWidgetItem("No");
            QTableWidgetItem *song_checkbox = new QTableWidgetItem("Yes");

            ui->tableWidget->setItem(song_pos + i, 0, song_checkbox);
            ui->tableWidget->setItem(song_pos + i, 1, lyrics_checkbox);
            ui->tableWidget->setItem(song_pos + i, 2, new QTableWidgetItem(name));
        }
    }

    return true;
}

bool MainWindow::createTracklistFile()
{
    QString list("echo \"--------------------------------------------------------\"\n");

    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QString i_str = QString::number(i + 1);
        list.append("echo \"song" + i_str + ": " + ui->tableWidget->item(i, 2)->text() + "\"\n");
    }

    list.append("echo \"--------------------------------------------------------\"\n");

    tracklist.write(list.toUtf8());
    return true;
}

bool MainWindow::addSongToConfig(const QString &filename, const QString &id)
{
    QString line;
    QFile source("lyrics/" + filename + ".txt");

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

    QString str = QString("alias song%1lyrics%2 say \"---THE END---\";alias spamycs;\n")
                        .arg(id, QString::number(i));
    dest.write(str.toUtf8());

    return true;
}

bool MainWindow::createSongIndex(const QString &id)
{
    QChar relay_key = '=';
    QString str("alias song" + id + " \"alias spamycs song" + id + "lyrics0;"
                "bind " + relay_key + " " + id + ";echo \"Current song: " + id + "\";"
                "host_writeconfig lyrics_trigger;\n");
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

        karaokePathsConf(dir);
    }
}

void MainWindow::on_refreshButton_clicked()
{
    refreshSongList();
}

void MainWindow::loadSong(int songid)
{
    QString s = "songs/"+ ui->tableWidget->item(songid - 1, 2)->text() + ".wav";
    QString d = hldir_root + "/voice_input.wav";

    if (QFileInfo::exists(d))
    {
        QFile::remove(d);
    }

    if (!QFile::copy(s, d))
        ui->err->setText("Couldnt copy song to game folder");
}

void MainWindow::checkConfigFile()
{
    if (QFileInfo::exists(userdatapath))
    {
        QFile f(userdatapath);
        f.open(QIODevice::ReadOnly);
        QString cfg = f.readAll();
        int start = cfg.indexOf("bind \"=\" ") + 10;
        int end = cfg.indexOf("\"", start + 1);
        f.remove();
        loadSong(cfg.mid(start, end - start).toInt());
    }
}

void MainWindow::on_startButton_clicked()
{
    if (ui->startButton->text() == "Start")
    {
        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(checkConfigFile()));
        timer->start(200);

        ui->startButton->setText("Stop");
    }
    else
    {
        if (QFileInfo::exists(hldir_root + "/voice_input.wav"))
        {
            QFile rm(hldir_root + "/voice_input.wav");
            rm.remove();
        }
        ui->startButton->setText("Start");
        timer->stop();
    }

    tracklist.open(QIODevice::WriteOnly | QIODevice::Truncate);
    dest.open(QIODevice::WriteOnly | QIODevice::Truncate);

    dest.write("alias spamycs say_team \"type exec lyrics_list.cfg in the console to see list with available songs\"\n");
    dest.write("alias karaoke_play karaoke_play_on\n"
               "alias karaoke_play_on \"alias karaoke_play karaoke_play_off;"
               " voice_inputfromfile 1; voice_loopback 1; +voicerecord\"\n"
               "alias karaoke_play_off \"-voicerecord; voice_inputfromfile 0;"
               " voice_loopback 0; alias karaoke_play karaoke_play_on\";bind x \"karaoke_play\"\n");
    createTracklistFile();

    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        addSongToConfig(ui->tableWidget->item(i, 2)->text(), QString::number(i + 1));
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

void MainWindow::handleLyricsSearchReply(QNetworkReply *reply)
{
    QString regex_str;
    bool isGenius = reply->url().toString().contains("genius.com");

    if (isGenius)
        regex_str = "\"full_title\":\"(?<song>.+?) by.(?<artist>.+?)\".+?\"path\":\"(?<link>.+?)\"";
    else
        regex_str = "\\d{1,2}. <a href=\"(?<link>https://www.azlyrics.com/lyrics/.+?html)\" target=\"_blank\"><b>(?<song>.+?)</b></a>  by <b>(?<artist>.+?)</b><br>";

    QRegularExpression regex = QRegularExpression( regex_str
                               , QRegularExpression::DotMatchesEverythingOption);

    QStringList items;
    QStringList links;

    QByteArray page = reply->readAll();

    QRegularExpressionMatchIterator i = regex.globalMatch(page);

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

            if (isGenius)
                lyrics_url.prepend("https://genius.com");

            getPage(lyrics_url);


            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Download song?","Download song?",
                                          QMessageBox::Yes|QMessageBox::No);

            if (reply == QMessageBox::Yes)
            {
                downloadSongYoutube(item);
            }
        }
    }
    else
    {
        if (reply->url().toString().contains("azlyrics"))
        {
            request.setUrl(QUrl("https://genius.com/api/search/song?page=1&q=" + search_string));
            manager->get(request);
            return;
        }

        QMessageBox msgBox;
        msgBox.setText("Couldn't find these lyrics online!");
        msgBox.exec();
    }
    search_string = "";
}

QString MainWindow::getGeniusSongName(const QString &page)
{
    QString song_title = " - ";

    int song_start = page.indexOf("class=\"header_with_cover_art-primary_info-title \"") + 61;
    int song_end = page.indexOf("<", song_start);
    song_title.append(page.mid(song_start, song_end - song_start));

    int artist_start = page.indexOf("class=\"header_with_cover_art-primary_info-primary_artist\"", song_start) + 71;
    int artist_end = page.indexOf("<", artist_start);
    song_title.prepend(page.mid(artist_start, artist_end - artist_start));

    return song_title;
}

bool MainWindow::handleLyricsReply(QNetworkReply *reply)
{
    QFile lyrics_file;
    QString lyrics;
    QString url = reply->url().toString();

    if (url.startsWith("https://www.azlyrics.com/lyrics/"))
    {
        QString page = reply->readAll();
        int start_pos = page.indexOf("<!-- Usage of azlyrics.com content") + 134;
        int end_pos = page.indexOf("</div>", start_pos);

        lyrics = page.mid(start_pos, end_pos - start_pos);
        lyrics.remove("<br>");
    }
    else if (QRegularExpression("^https://genius.com/[A-Za-z0-9_-]+$").match(url).hasMatch())
    {
        QString page = reply->readAll();
        int start = page.indexOf("<!--sse-->", page.indexOf("<!--sse-->") + 1) + 26;
        int end = page.indexOf("<!--/sse-->", start) - 19;
        lyrics = page.mid(start, end - start);
        QRegularExpression regexp("</a>|</b>|<b>|<br>|<a.+?\">");
        lyrics = lyrics.remove(regexp);
        temp_lyrics_name = getGeniusSongName(page);
    }
    else
    {
        return false;
    }

    lyrics_file.setFileName("lyrics/" + temp_lyrics_name + ".txt");

    if (lyrics_file.open(QIODevice::WriteOnly))
    {
        lyrics_file.write(lyrics.toUtf8());
        lyrics_file.close();
    }
    else
        qDebug() << lyrics_file.errorString();

    refreshSongList();

    temp_lyrics_name = "";
    return true;
}

void MainWindow::getPage(const QString &url)
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

    if (handleLyricsReply(reply))
        return;

    handleLyricsSearchReply(reply);
}

void MainWindow::on_deleteSongButton_clicked()
{
    ui->err->setText("");

    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.size() == 0)
        return;

    QString question_content = QString("Are you sure you want to delete %1 song's lyrics?")
            .arg(QString::number(items.size()/3));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete songs", question_content,
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes)
    {
        for (int i = 0; i < items.size()/3; i++)
        {
            QString name = items.at(i * 3 + 2)->text();
            if (QFileInfo::exists("lyrics/" + name + ".txt"))
                if (!QFile::remove("lyrics/" + name + ".txt"))
                    ui->err->setText("Permission error!");
            if (QFileInfo::exists("songs/" + name + ".wav"))
                QFile::remove("songs/" + name + ".wav");
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
    search_string = text;

    if (ok && !text.isEmpty())
    {
        request.setUrl(QUrl("https://search.azlyrics.com/search.php?q=" + text + "&w=songs&p=1"));
        manager->get(request);
    }
}

void MainWindow::songCooked()
{
    QDir root;
    root.setNameFilters(QStringList() << "*.wav" << "*.webm.part" << "*.webm");
    root.setFilter(QDir::Files);

    bool success = false;

    if (root.count() > 0)
    {
        QStringList ffmpegsux = root.entryList();

        for (int i = 0; i < ffmpegsux.size(); i++)
        {
            QString filename = ffmpegsux.at(i);
            if (filename.endsWith(".wav"))
            {
                dl_file_name.replace("_", " ");
                QFile::rename(filename, "songs/" + dl_file_name + ".wav");
                success = true;
            }
            else
            {
                if (QFileInfo::exists(filename))
                    QFile::remove(filename);
            }
        }

        QMessageBox msgbox;
        if (success)
            msgbox.setText("Song Downloaded!");
        else
            msgbox.setText("Song Failed to download!");
        msgbox.exec();

        dl_file_timer->stop();

        refreshSongList();
    }
}

void MainWindow::downloadSongYoutube(const QString &params)
{
    QProcess *process = new QProcess(this);

    QString search_str;
    QString name_str;

    name_str = params;
    name_str.replace(" ", "_");

    if (params.startsWith("https://"))
        search_str = params;
    else
        search_str = "\"ytsearch: " + params + "\"";

    QString program = "youtube-dl.exe -x --extract-audio --audio-format wav " + search_str + " "
                      "--postprocessor-args \"-fflags +bitexact -ac 1 -ab 352k -ar 22050\"";

    dl_file_timer = new QTimer(this);
    dl_file_name = name_str;
    connect(dl_file_timer, SIGNAL(timeout()), this, SLOT(songCooked()));

    process->start(program);
    process->waitForFinished(10000);
    process->close();
    dl_file_timer->start(800);
}

void MainWindow::on_youtubeButton_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Youtube download"),
                                    tr("Type song name or link"), QLineEdit::Normal,
                                    QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
    {
        downloadSongYoutube(text);
    }
}
