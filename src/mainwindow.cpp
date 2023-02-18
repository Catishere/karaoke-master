#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectUi();
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setColumnWidth(0, 40);
    ui->tableWidget->setColumnWidth(1, 40);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::showContextMenu);

    movie = new QMovie(":/loading.gif");
    movie->setScaledSize(QSize(300, 270));
    ui->loadingLabel->setVisible(false);
    ui->loadingLabel->setMovie(movie);

    ui->progressBar->setVisible(false);

    if (!configController.getConfigEntries().isEmpty()) {
        ui->startButton->setEnabled(true);
        updateAccount();
        refreshScriptPaths();
        loadDropListPaths();
    }

    fetchers_ready = 0;
    sayType = "say";
    auto currentConfig = configController.getCurrentConfig();
    timer_interval = (currentConfig) ? getTimerInterval(currentConfig->getPc()):
                                       200;

    all_fetchers = { new GeniusLyricsFetcher(this),
                     new LyricstranslateFetcher(this),
                     new MusixmatchFetcher(this) };

    temp_search_list.resize(all_fetchers.size());

    updateAllowedFetchers();

    QDir root(".");
    root.mkdir("lyrics");
    root.mkdir("songs");
    root.mkdir("config");

    updateManager = new UpdateManager(this);
    connect(updateManager, &UpdateManager::YTDLUpdateReady,
            this, &MainWindow::handleYTDLUpdateResponse);
    connect(updateManager, &UpdateManager::downloadProgress,
            this, &MainWindow::downloadProgress);

    showUpdateNotification();

    setWindowIcon(QIcon(":/icon/favicon.ico"));

    connect(&configController, &ConfigController::listChanged,
            this, &MainWindow::loadDropListPaths);

    refreshSongList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::refreshSongList()
{
    auto lyrics = QDir("lyrics").entryList(QStringList("*.txt"), QDir::Files);
    auto songs = QDir("songs").entryList(QStringList("*.wav"), QDir::Files);
    auto table = ui->tableWidget;

    int shared = 0;
    for (auto& lyric : lyrics)
        if (songs.contains(lyric.chopped(4) + ".wav"))
            shared++;

    table->clearContents();
    table->setRowCount(lyrics.size() + songs.size() - shared);

    for (int i = 0; i < lyrics.size(); i++) {
        QString name = lyrics.at(i).chopped(4);
        auto hasSong = QFileInfo::exists("songs/" + name + ".wav") ? "Yes":"No";

        table->setItem(i, 0, new QTableWidgetItem(hasSong));
        table->setItem(i, 1, new QTableWidgetItem("Yes"));
        table->setItem(i, 2, new QTableWidgetItem(name));
    }

    int index = lyrics.size();

    for (auto& song_raw : songs) {
        QString song = song_raw.chopped(4);
        if (lyrics.contains(song + ".txt")) continue;
        table->setItem(index, 0, new QTableWidgetItem("Yes"));
        table->setItem(index, 1, new QTableWidgetItem("No"));
        table->setItem(index, 2, new QTableWidgetItem(song));
        index++;
    }
}

bool MainWindow::createTracklistFile()
{
    QString list("exec lyricsmaster.cfg;\n");
    const char* border =
          "echo \"--------------------------------------------------------\"\n";
    list.append(border);
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QString i_str = QString::number(i + 1);
        list.append("echo \"song"
                    + i_str + ": "
                    + ui->tableWidget->item(i, 2)->text()
                    + "\"\n");
    }
    list.append(border);

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
            QString cfg_line = QString("alias song%1lyrics%2 \"%3 ~ %4 ;"
                                       "alias spamycs song%5lyrics%6\"\n")
                 .arg(id, QString::number(i), sayType, line,
                      id, QString::number(i + 1));
            dest.write(cfg_line.toUtf8());
        }

        else i--;
    }

    QString str = QString("alias song%1lyrics%2 %3 \"---THE END---\";\n")
                        .arg(id, QString::number(i), sayType);
    dest.write(str.toUtf8());

    return true;
}

bool MainWindow::createSongIndex(const QString &id)
{
    QChar relay_key = '=';
    QString compatWriteCfg = "host_writeconfig";
    if (configController.getCurrentConfig()->getFullName() == "Half-Life")
        compatWriteCfg = "writecfg";

    QString songname = ui->tableWidget->item(id.toInt() - 1, 2)->text();
    QString str = QStringLiteral("alias say_song%2 \"%4 Current Song: %1\";"
                                 "alias song%2 \"alias spamycs song%2lyrics0;"
                                 "bind %3 %2; alias lyrics_current say_song%2;"
                                 "%5 lyrics_trigger;\"\n")
                .arg(songname, id, relay_key, sayType, compatWriteCfg);

    dest.write(str.toUtf8());
    return true;
}

void MainWindow::directoryClicked()
{
    if (configController.getConfigEntries().isEmpty()) {
        auto reply = QMessageBox::question(this, "Automatic game finder",
                              "It's your first time adding configs, do you want"
                              " the app to find the games automatically?");
        if (reply == QMessageBox::StandardButton::Yes) {
            findGamesTriggered();
            return;
        }
    }
    QString path = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                            "/",
                                            QFileDialog::ShowDirsOnly
                                            | QFileDialog::DontResolveSymlinks);
    if (path.isEmpty()) return;

    QDir dir(path);
    if (dir.dirName() == "cfg" || dir.entryList().contains("config.cfg")) {
        ui->startButton->setEnabled(true);

        configController.addConfig(ConfigEntry(path));
        configController.saveConfig();
        refreshScriptPaths();
        loadDropListPaths();
        QTimer::singleShot(200, this, [=](){updateAccount();});
    } else
        QMessageBox::warning(this, "Config",
                             "That is not valid source configuration folder.");
}

void MainWindow::refreshClicked()
{
    refreshSongList();
}

void MainWindow::loadSong(int songid)
{
    QString s = "songs/"+ ui->tableWidget->item(songid - 1, 2)->text() + ".wav";
    QString d = configController.getCurrentGamePath() + "/voice_input.wav";

    if (QFileInfo::exists(d))
        QFile::remove(d);

    if (!QFile::copy(s, d))
        ui->err->setText("Couldnt copy song to game folder");
}

void MainWindow::checkConfigFile()
{
    QString ud = configController.getUserDataPath();
    if (configController.getCurrentConfig()->getFullName() == "Half-Life")
        ud = configController.getCurrentGamePath() + "/lyrics_trigger.cfg";

    if (QFileInfo::exists(ud)) {
        QFile f(ud);
        f.open(QIODevice::ReadOnly);
        QString cfg = f.readAll();
        int start = cfg.indexOf("bind \"=\" ") + 10;
        int end = cfg.indexOf("\"", start + 1);
        f.remove();
        loadSong(cfg.mid(start, end - start).toInt());
    }
}
void MainWindow::updateConfigSongList()
{
    tracklist.open(QIODevice::WriteOnly | QIODevice::Truncate);
    dest.open(QIODevice::WriteOnly | QIODevice::Truncate);

    auto keys = configController.getCurrentConfig()->getKeyBindings();
    QString voice_command;
    QString lyrics_command;
    for (auto& key : keys) {
        if (key.first == "Voice")
            voice_command = key.second;
        else if (key.first == "Lyrics")
            lyrics_command = key.second;
    }
    dest.write(QString("alias spamycs say_team \"type exec lyrics_list.cfg in "
                       "the console to see list with available songs\"\nalias "
                       "karaoke_play karaoke_play_on\nalias karaoke_play_on "
                       "\"alias karaoke_play karaoke_play_off;"
                       "voice_inputfromfile 1;voice_loopback 1;+voicerecord\"\n"
                       "alias karaoke_play_off \"-voicerecord; "
                       "voice_inputfromfile 0; voice_loopback 0; alias "
                       "karaoke_play karaoke_play_on\";"
                       "bind " + voice_command + " \"karaoke_play\";"
                       "bind " + lyrics_command + " spamycs\n").toUtf8());
    createTracklistFile();

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        addSongToConfig(ui->tableWidget->item(i, 2)->text(),
                        QString::number(i + 1));
        createSongIndex(QString::number(i+1));
    }

    tracklist.close();
    dest.close();
}

void MainWindow::startClicked()
{
    if (ui->startButton->text() == "Start") {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &MainWindow::checkConfigFile);
        timer->start(200);

        ui->startButton->setText("Stop");
        updateConfigSongList();
    }
    else {
        QString gamepath = configController.getCurrentGamePath();
        if (QFileInfo::exists(gamepath + "/voice_input.wav")) {
            QFile rm(gamepath + "/voice_input.wav");
            rm.remove();
        }
        ui->startButton->setText("Start");
        timer->stop();
    }
}

void MainWindow::addSongClicked()
{
    ui->err->setText("");

    QStringList songs_list = QFileDialog::getOpenFileNames(this,
                                                   tr("Open Directory"),
                                                   "/",
                                                   tr("Text Files (*.txt)"));
    for (int i = 0; i < songs_list.size(); i++) {
        QString song = songs_list.at(i);
        int pos = song.lastIndexOf(QChar('/'));
        QString song_newpath = "lyrics" + song.right(song.size() - pos);

        QFile song_file(song);
        if (!song_file.copy(song_newpath))
            ui->err->setText(song_file.errorString());
    }
    refreshSongList();
}

void MainWindow::handleYTDLUpdateResponse(Response response)
{
    switch (response) {
    case Response::UPDATED:
        QMessageBox::information(this, "YTDL Update",
                                 "yt-dl.exe has been updated!");
        break;
    case Response::UP_TO_DATE:
        QMessageBox::information(this, "YTDL Update",
                                 "Skipping. Already up to date!");
        break;
    case Response::FAILED:
        QMessageBox::critical(this, "YTDL Update",
                                 "Update failed!");
    }
    ui->progressBar->setVisible(false);
}

void MainWindow::deleteSongClicked()
{
    ui->err->setText("");

    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();

    if (items.size() == 0) {
        QMessageBox::information(this,
                                 "Delete songs",
                                 "You haven't selected any songs.");
        return;
    }

    QString question_content =
            QString("Are you sure you want to delete %1 song's lyrics?")
            .arg(QString::number(items.size()/3));

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete songs", question_content,
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        for (int i = 0; i < items.size()/3; i++) {
            QString name = items.at(i * 3 + 2)->text();
            if (QFileInfo::exists("lyrics/" + name + ".txt")
                && !QFile::remove("lyrics/" + name + ".txt")) {
                ui->err->setText("Permission error!");
            }
            if (QFileInfo::exists("songs/" + name + ".wav"))
                QFile::remove("songs/" + name + ".wav");
        }
        refreshSongList();
    }
}

void MainWindow::searchOnlineClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Online Search"),
                            tr("Type song name, artist or part of the song"),
                            QLineEdit::Normal,
                            QDir::home().dirName(), &ok);
    search_string = text;

    if (ok && !text.isEmpty()) {
        movie->start();
        ui->loadingLabel->setVisible(true);

        for (auto &fetcher : allowed_fetchers)
            fetcher->fetchList(text);

        search_string = "";
    }
}

void MainWindow::songCooked()
{
    if (timeout_timer->isActive())
        timeout_timer->stop();

    delete timeout_timer;

    QDir root;
    root.setNameFilters(QStringList() << "*.wav" << "*.webm.part" << "*.webm");
    root.setFilter(QDir::Files);

    bool success = false;

    if (root.count() > 0) {
        QStringList ffmpegsux = root.entryList();

        for (int i = 0; i < ffmpegsux.size(); i++) {
            QString filename = ffmpegsux.at(i);
            if (filename.endsWith(".wav")) {
                dl_file_name.replace(QChar(0x00A0), " ");
                QFile::rename(filename, "songs/" + dl_file_name + ".wav");
                success = true;
            }
            else if (QFileInfo::exists(filename))
                QFile::remove(filename);
        }
    }

    movie->stop();
    ui->loadingLabel->setVisible(false);

    if (success) {
        QTimer::singleShot(100, this, &MainWindow::startClicked);
        QTimer::singleShot(200, this, &MainWindow::startClicked);
    }
    else
        QMessageBox::warning(this, "Song Download", "Song Failed to download!");

    refreshSongList();
}

void MainWindow::downloadSongYoutube(QString &song_name)
{
    QProcess *process = new QProcess(this);

    QString search_str;

    if (song_name.startsWith("https://"))
        search_str = song_name;
    else {
        song_name.replace(QRegularExpression("[%.\\/: ]"), " ");
        search_str = "\"ytsearch: " + song_name + "\"";
    }
    QString program = "yt-dlp.exe -x --extract-audio --audio-format wav "
                      + search_str +
                      " --ppa \"ffmpeg: -bitexact -ac 1 -ab 352k -ar 22050\"";

    dl_file_name = song_name;
    timeout_timer = new QTimer(this);
    timeout_timer->setInterval(10000);
    timeout_timer->setSingleShot(true);
    connect(timeout_timer, &QTimer::timeout, this, &MainWindow::songCooked);
    connect(process, &QProcess::finished, this, &MainWindow::songCooked);
    timeout_timer->start();
    movie->start();
    ui->loadingLabel->setVisible(true);
    process->start(program);
}

void MainWindow::youtubeClicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Youtube download"),
                                         tr("Type song name or link"),
                                         QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty())
        downloadSongYoutube(text);
}

void MainWindow::loadDropListPaths()
{
    ui->dropList->clear();
    for (const auto& config : configController.getConfigEntries())
        ui->dropList->addItem(config.getFullName(), config.getName());

    int index = ui->dropList->findData(configController
                                       .getCurrentConfig()
                                       ->getName());
    if (index != -1)
        ui->dropList->setCurrentIndex(index);

    if (configController.getConfigEntriesRef().isEmpty())
        ui->startButton->setDisabled(true);
}

void MainWindow::refreshScriptPaths()
{
    ConfigEntry *config = configController.getCurrentConfig();

    tracklist.setFileName(config->getPath() + "/lyrics_list.cfg");
    dest.setFileName(config->getPath() + "/lyricsmaster.cfg");
}

const QStringList MainWindow::getMostRecentUser() const
{
    QString udpath = configController.getUserDataPath();

    if (udpath.isNull() || udpath.isEmpty())
        return QStringList();

    QString steamPath = udpath.left(udpath.indexOf("/Steam/") + 7);
    QString steamConfig = steamPath + "/config/loginusers.vdf";
    QFile steamConfigFile(steamConfig);

    if (!steamConfigFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open steam config file.");
        return QStringList();
    }

    QByteArray file_data = steamConfigFile.readAll();

    QStringList result;

    QString regex_str = "\"(?<steamid64>\\d+)\"\\s+?{\\s+?\"AccountName\"\\"
                        "s+?\"(?<username>.+?)\".+?\"MostRecent\"\\"
                        "s+\"(?<mostrecent>\\d)\".+?}";
    QRegularExpression regex = QRegularExpression( regex_str,
                               QRegularExpression::DotMatchesEverythingOption);

    QRegularExpressionMatchIterator i = regex.globalMatch(file_data);

    while (true) {
        if (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            QString steamid64 = match.captured("steamid64");
            QString username = match.captured("username");
            QString mostrecent = match.captured("mostrecent");

            if (mostrecent == "1") {
                qlonglong num = (steamid64.toLongLong() & 0xFFFFFFFF);
                result << username << QString::number(num);
                break;
            }
        }
        else
            break;
    }

    return result;
}

void MainWindow::updateAccount()
{
    QStringList user = getMostRecentUser();
    if (user.isEmpty()) return;
    ui->account->setText(user.at(0));
    configController.setAccountId(user.at(1));
}

void MainWindow::tsayChanged(int state)
{
    sayType = state ? "say_team" : "say";
}

void MainWindow::updatedYTDLTriggered()
{
    updateManager->updateYTDL();
}


void MainWindow::updateAccountTriggered()
{
    updateAccount();
}


void MainWindow::aboutTriggered()
{
    QMessageBox::about(this, "About", "This is Karaoke master version " VERSION
                                      ". Made by Victor G.");
}

void MainWindow::guideTriggered()
{
    QMessageBox::information(this, "Guide",
    "Press 'Choose config' and choose your game cfg folder to begin.<br>"
    "Type exec lyrics_list.cfg in the console to see the current loaded songs."
    "<br>Use the settings menu to choose which keys you want to bind.<br>"
    "Press Start and become a DJ.<br>More info "
    "<a href='https://github.com/Catishere/karaoke-master'>here</a>.");
}

void MainWindow::updateClientTriggered()
{
    if (QFileInfo::exists("karaoke-master-update.exe")) {
        auto const conn = new QMetaObject::Connection;
        *conn = connect(updateManager, &UpdateManager::finished,
                        this, [this, conn](const QByteArray info) {
            QObject::disconnect(*conn);
            delete conn;
            QJsonDocument doc = QJsonDocument::fromJson(info);

            if (doc["tag_name"].toString() == VERSION) {
                QMessageBox::information(this, "Update",
                                         "You are running the latest update.");
                return;
            }

            configController.setUpdateNotification(false);
            configController.saveConfig();

            qApp->quit();
            QProcess::startDetached("karaoke-master-update.exe");
        });
        updateManager->getUpdateInfo();
    } else {
        QMessageBox::information(this, "Update",
            "You don't have karaoke-master-update.exe. "
            "You can download the package "
            "<a href='https://github.com/Catishere/karaoke-master/"
            "releases/latest'>here</a>.");
    }
}


void MainWindow::keyBindingsTriggered()
{
    auto config = configController.getCurrentConfig();
    if (config == nullptr) {
        QMessageBox::warning(this, "Key bindings",
                             "Choose your game cfg folder first.");
        return;
    }

    bool ok;
    InputDialog id(this, config->getKeyBindings(), "Key Bindings");
    StringPairList list = id.getStrings(&ok);
    if (ok) {
        config->setKeyBindings(list);
        configController.saveConfig();
    }
}

void MainWindow::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), ui->tableWidget);
    QList<QAction*> actions = {
        new QAction("Delete selected songs", ui->tableWidget),
        new QAction("Add song with lyrics", ui->tableWidget),
        new QAction("Add song without lyrics", ui->tableWidget),
        new QAction("Add song from local folder", ui->tableWidget)
    };

    auto actionSlots = QList {
        &MainWindow::deleteSongClicked,
        &MainWindow::searchOnlineClicked,
        &MainWindow::youtubeClicked,
        &MainWindow::addSongClicked
    };

    for (int i = 0; i < actions.size(); ++i) {
        connect(actions.at(i), &QAction::triggered,
                this, actionSlots[i]);
        contextMenu.addAction(actions[i]);
    }

    contextMenu.exec(ui->tableWidget->pos() + mapToGlobal(pos) + QPoint(0, 45));
}

int MainWindow::getTimerInterval(const QString pc)
{
    if (pc == "Potato")
        return 2000;
    if (pc == "Slow")
        return  1000;
    if (pc ==  "Average")
        return 500;
    if (pc == "Fast")
        return 200;
    if (pc == "Alien")
        return 200;
    return 500;
}

void MainWindow::optionsTriggered()
{
    auto config = configController.getCurrentConfig();
    if (config == nullptr) {
        QMessageBox::warning(this, "Options",
                             "Choose your game cfg folder first.");
        return;
    }

    OptionsDialog *optionsDialog = new OptionsDialog(this, &configController,
                                                     &all_fetchers);
    connect(optionsDialog, &OptionsDialog::updateAllowedFetchers,
            this, &MainWindow::updateAllowedFetchers);
    optionsDialog->exec();
}

void MainWindow::allLyricsListsFetched()
{
    fetchers_ready = 0;

    movie->stop();
    ui->loadingLabel->setVisible(false);

    for (auto& list : temp_search_list) {
        search_list.append(list);
    }

    temp_search_list.clear();
    temp_search_list.resize(all_fetchers.size());

    if (search_list.isEmpty()) {
        QMessageBox::information(this, "Lyrics",
                                 "Couldn't find these lyrics online!");
        return;
    }

    QDialog *dialog = new QDialog(this);
    QGridLayout *layout = new QGridLayout(dialog);
    auto comboBox = new QComboBox(dialog);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                          | QDialogButtonBox::Cancel);
    for (auto &pair : search_list) {
        comboBox->addItem(QIcon(":icon/" + pair.first.right(2) + ".ico"),
                          pair.first.chopped(2));
    }

    layout->addWidget(comboBox);
    layout->addWidget(buttonBox);
    dialog->setWindowTitle("Online Search");
    dialog->setLayout(layout);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [=](){
        QString item = comboBox->currentText();
        qsizetype item_index = comboBox->currentIndex();

        if (item.isEmpty()) {
            dialog->close();
            return;
        }

        temp_lyrics_name = item;

        for (auto &fetcher : allowed_fetchers)
            fetcher->fetchLyrics(search_list.at(item_index).second);

        search_list.clear();

        if (configController.getCurrentConfig()->getAlwaysDownload()
            || QMessageBox::question(this, "Download song?", "Download song?",
               QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
            downloadSongYoutube(item);

        dialog->close();
    });

    connect(buttonBox, &QDialogButtonBox::rejected, this, [=](){
        search_list.clear();
        dialog->close();
    });

    dialog->exec();
}

void MainWindow::addListWithPriorty(const StringPairList &list)
{
    if (list.isEmpty())
        return;
    auto url = list.at(0).second;

    for (int i = 0; i < allowed_fetchers.size(); ++i) {
        if (url.startsWith(allowed_fetchers[i]->getEndpoint())) {
            temp_search_list[i] = list;
        }
    }
}

void MainWindow::showUpdateNotification()
{
    auto const conn = new QMetaObject::Connection;
    *conn = connect(updateManager, &UpdateManager::finished,
                          this, [this, conn](const QByteArray info) {
        QObject::disconnect(*conn);
        delete conn;
        QJsonDocument doc = QJsonDocument::fromJson(info);
        QString version = doc["tag_name"].toString();
        QString changes = doc["body"].toString();

        if (version == VERSION) {
            if (configController.isUpdateNotification()) {
                configController.setUpdateNotification(true);
                configController.saveConfig();
                QMessageBox::information(this, "Updated",
                                         "The application is successfully"
                                         " updated to version " VERSION
                                         ".\n New things: \n" + changes);
            }
            return;
        }
        auto reply = QMessageBox::question(this, "Update available",
                                           "Update for version " + version + " "
                                           "is available (Current " VERSION ")."
                                           " Do you want to update?",
                                           QMessageBox::Yes | QMessageBox::No,
                                           QMessageBox::Yes);
        if (reply == QMessageBox::Yes)
            updateClientTriggered();
    });

    updateManager->getUpdateInfo();
}

void MainWindow::updateAllowedFetchers()
{
    allowed_fetchers.clear();
    auto allowed = configController.getAllowedFetchers();

    if (allowed.isEmpty()) {
        configController.setAllowedFetchers({
                                                {"Genius", true},
                                                {"Lyrics Translate", true},
                                                {"Musixmatch", true}
                                            });
        allowed = configController.getAllowedFetchers();
    }

    for (auto& f : all_fetchers) {
        disconnect(dynamic_cast<QObject*>(f), nullptr,
                   nullptr, nullptr);
        auto isAllowed = allowed.constFind(f->getFullName());
        if (isAllowed != allowed.constEnd()
            && isAllowed.value())
            allowed_fetchers.append(f);
    }

    for (auto& fet : allowed_fetchers) {
        connect(dynamic_cast<QObject*>(fet), SIGNAL(listReady(StringPairList)),
                this, SLOT(lyricsListFetched(StringPairList)));
        connect(dynamic_cast<QObject*>(fet), SIGNAL(lyricsReady(QString)),
                this, SLOT(lyricsFetched(QString)));
    }
}

void MainWindow::connectUi()
{
    connect(ui->directoryButton, &QPushButton::clicked,
            this, &MainWindow::directoryClicked);
    connect(ui->refreshButton, &QPushButton::clicked,
            this, &MainWindow::refreshClicked);
    connect(ui->startButton, &QPushButton::clicked,
            this, &MainWindow::startClicked);
    connect(ui->addSongButton, &QPushButton::clicked,
            this, &MainWindow::addSongClicked);
    connect(ui->deleteSongButton, &QPushButton::clicked,
            this, &MainWindow::deleteSongClicked);
    connect(ui->searchOnlineButton, &QPushButton::clicked,
            this, &MainWindow::searchOnlineClicked);
    connect(ui->youtubeButton, &QPushButton::clicked,
            this, &MainWindow::youtubeClicked);
    connect(ui->dropList, &QComboBox::textActivated,
            this, &MainWindow::dropListChanged);
    connect(ui->tsayCheckBox, &QCheckBox::stateChanged,
            this, &MainWindow::tsayChanged);
    connect(ui->actionUpdate_YTDL, &QAction::triggered,
            this, &MainWindow::updatedYTDLTriggered);
    connect(ui->actionUpdate_account_info, &QAction::triggered,
            this, &MainWindow::updateAccountTriggered);
    connect(ui->actionAbout, &QAction::triggered,
            this, &MainWindow::aboutTriggered);
    connect(ui->actionGuide, &QAction::triggered,
            this, &MainWindow::guideTriggered);
    connect(ui->actionUpdate_client, &QAction::triggered,
            this, &MainWindow::updateClientTriggered);
    connect(ui->actionKey_bindings, &QAction::triggered,
            this, &MainWindow::keyBindingsTriggered);
    connect(ui->actionOptions, &QAction::triggered,
            this, &MainWindow::optionsTriggered);
    connect(ui->actionFind_games, &QAction::triggered,
            this, &MainWindow::findGamesTriggered);
}

void MainWindow::lyricsListFetched(const StringPairList &list)
{
    addListWithPriorty(list);
    fetchers_ready++;
    if (fetchers_ready >= allowed_fetchers.size())
        allLyricsListsFetched();
}

void MainWindow::lyricsFetched(const QString& lyrics)
{
    if (lyrics.isEmpty()) {
        QMessageBox::warning(this, "Lyrics failed",
                              "The lyrics are empty, try with another source");
        return;
    }
    qDebug() << "Saving lyrics...";
    QFile lyrics_file;

    temp_lyrics_name.replace(QChar(0xA0), " ");
    temp_lyrics_name.replace(QRegularExpression("[%.\\/:]"), " ");

    lyrics_file.setFileName("lyrics/" + temp_lyrics_name + ".txt");

    if (lyrics_file.open(QIODevice::WriteOnly)) {
        lyrics_file.write(lyrics.toUtf8());
        lyrics_file.close();
    }
    else
        qDebug() << lyrics_file.errorString();

    refreshSongList();
}

void MainWindow::downloadProgress(qint64 ist, qint64 max)
{
    ui->progressBar->setVisible(true);
    ui->progressBar->setRange(0, max);
    ui->progressBar->setValue(ist);
    if (max < 0) ui->progressBar->setVisible(false);
}


void MainWindow::dropListChanged(const QString &arg1)
{
    configController.choose(arg1);
    refreshScriptPaths();
}

QDir findFolder(const QDir dir, int depth) {
    if (depth < 0)
        return QDir("/");
    for (const auto& folder : dir.entryList(QDir::AllEntries
                                            | QDir::NoDotAndDotDot)) {
        if (folder == "Steam")
            return dir;

        auto newDir = findFolder(QDir(dir.path() + folder + '/'), depth - 1);
        if (newDir.path() != "/") {
            newDir.setPath(newDir.path() + STEAMAPPS);
            return newDir;
        }
    }
    return QDir("/");
}

void MainWindow::findGamesTriggered()
{
    QStringList gameDirs;
    QMap<QString, QString> games {
        { "Half-Life",                       "/cstrike" },
        { "Half-Life 2",                     "/hl2/cfg" },
        { "Counter-Strike Global Offensive", "/csgo/cfg" },
        { "GarrysMod",                       "/garrysmod/cfg" },
        { "Counter-Strike Source",           "/cstrike/cfg" },
    };

    for (auto& drive : QDir::drives()) {
        auto dir = findFolder(QDir(drive.absolutePath()), 3);
        for (auto& e : dir.entryInfoList(QDir::AllEntries
                                         | QDir::NoDotAndDotDot)) {
            if (games.contains(e.fileName()))
                gameDirs.append(e.absoluteFilePath() + games[e.fileName()]);
        }
    }

    if (gameDirs.isEmpty()) {
        QMessageBox::warning(this, "No games found", "No games were found");
        return;
    }

    for (auto& dir : gameDirs)
        configController.addConfig(ConfigEntry(dir));

    configController.saveConfig();

    QMessageBox::information(this, "Games found", "Your games were loaded");
    ui->startButton->setEnabled(true);
    refreshScriptPaths();
    loadDropListPaths();
    QTimer::singleShot(200, this, [=](){updateAccount();});
}

