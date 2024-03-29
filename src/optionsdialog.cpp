#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent, ConfigController *configCtrl,
                             QList<LyricsFetcher *> *all_fetchers)
    :  QDialog(parent), all_fetchers(all_fetchers)
{
    auto config = configCtrl->getCurrentConfig();

    QVBoxLayout *lytMain = new QVBoxLayout(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    QHBoxLayout *fetchersHbox = new QHBoxLayout();

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Close);

    QCheckBox *checkbox = new QCheckBox("Always download song", this);
    QGroupBox *groupBox = new QGroupBox(tr("Your PC speed"), this);
    QGroupBox *fetchersGroupBox = new QGroupBox(tr("Allow Fetchers"), this);
    QList<QRadioButton *> buttons = {
        new QRadioButton(tr("Potato"), this),
        new QRadioButton(tr("Slow"), this),
        new QRadioButton(tr("Average"), this),
        new QRadioButton(tr("Fast"), this),
        new QRadioButton(tr("Alien"), this)
    };

    QString currentPC = config->getPc();
    checkbox->setChecked(config->getAlwaysDownload());
    QMap<QString, bool> configFetcherList;

    if (configCtrl->getAllowedFetchers().isEmpty())
    {
        QMap<QString, bool> map;
        for (auto& f : *all_fetchers) {
            map.insert(f->getFullName(), true);
        }
        configCtrl->setAllowedFetchers(map);
        configCtrl->saveConfig();
        configFetcherList = map;
    } else
        configFetcherList = configCtrl->getAllowedFetchers();

    for (auto& f : *all_fetchers) {
        auto cb = new QCheckBox(f->getFullName());
        cb->setChecked(configFetcherList.find(f->getFullName()).value());
        fetchersHbox->addWidget(cb);
    }

    for (auto button : buttons) {
        hbox->addWidget(button);
        if (button->text() == currentPC)
            button->setChecked(true);
    }

    auto priorityButton = new QPushButton("Fetcher priority", this);
    auto manageConfigsButton = new QPushButton("Manage configs", this);

    connect(priorityButton, &QPushButton::pressed,
            this, &OptionsDialog::openFetcherPriorityDialog);

    ConfigDialog *configDialog = new ConfigDialog(this, configCtrl);
    connect(manageConfigsButton, &QPushButton::pressed,
            configDialog, &ConfigDialog::show);

    hbox->addStretch(1);
    fetchersGroupBox->setLayout(fetchersHbox);
    groupBox->setLayout(hbox);
    lytMain->addWidget(groupBox);
    lytMain->addWidget(fetchersGroupBox);
    lytMain->addWidget(priorityButton);
    lytMain->addWidget(manageConfigsButton);
    lytMain->addWidget(checkbox);
    lytMain->addWidget(buttonBox);
    setWindowTitle("Options");
    setLayout(lytMain);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [=]() {
        config->setAlwaysDownload(checkbox->isChecked());
        QMap<QString, bool> cfgFetchers;
        for (int i = 0; i < (*all_fetchers).size(); ++i) {
            auto widget = static_cast<QCheckBox *>(fetchersHbox->itemAt(i)
                                                   ->widget());
            cfgFetchers.insert(widget->text(), widget->isChecked());
        }
        configCtrl->setAllowedFetchers(cfgFetchers);
        emit updateAllowedFetchers();

        for (auto& button : buttons) {
            if (button->isChecked()) {
                config->setPc(button->text());
            }
        }
        configCtrl->saveConfig();
        close();
    });

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, [this]() { close(); });
}

void OptionsDialog::openFetcherPriorityDialog()
{
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *lytMain = new QVBoxLayout(dialog);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                                       QDialogButtonBox::Close);
    QListWidget *lw = new QListWidget(dialog);
    for (auto fetcher : *all_fetchers)
        lw->addItem(new QListWidgetItem(fetcher->getFullName()));
    lw->setDragDropMode(QAbstractItemView::InternalMove);
    lytMain->addWidget(new QLabel("Drag and drop items to reorder", this));
    lytMain->addWidget(lw);
    lytMain->addWidget(buttonBox);
    dialog->setWindowTitle("Fetcher priority");
    dialog->setLayout(lytMain);

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, [=]() {
        for(int i = 0; i < lw->count(); ++i)
        {
            auto item = lw->item(i);
            for (int j = 0; j < (*all_fetchers).size(); ++j) {
                auto fetcher = (*all_fetchers).at(j);
                if (fetcher->getFullName() == item->text())
                    (*all_fetchers).swapItemsAt(i, j);
            }
        }
        dialog->close();
    });

    connect(buttonBox, &QDialogButtonBox::rejected,
            this, [=]() { dialog->close(); });

    dialog->exec();
}
