#include "configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent, ConfigController *config)
    : QDialog(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    QHBoxLayout *labels = new QHBoxLayout();
    QVBoxLayout *centerButtons = new QVBoxLayout();
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Close);
    auto addList = new ConfigListWidget(this);
    auto trashList = new ConfigListWidget(this);

    for (auto& entry : config->getConfigEntries()) {
        auto item = new QListWidgetItem(entry.getFullName());
        QVariant v(entry.getPath());
        item->setData(Qt::UserRole, v);
        addList->addItem(item);
    }

    labels->addWidget(new QLabel("Keep these"));
    labels->addSpacing(75);
    labels->addWidget(new QLabel("Remove those"));

    centerButtons->addStretch();
    centerButtons->addWidget(new QPushButton("<",this));
    centerButtons->addWidget(new QPushButton(">",this));
    centerButtons->addStretch();
    hbox->addWidget(addList);
    hbox->addLayout(centerButtons);
    hbox->addWidget(trashList);
    vbox->addLayout(labels);
    vbox->addLayout(hbox);
    vbox->addWidget(buttons);
    setWindowTitle("Manage configs");
    setLayout(vbox);

    connect(buttons, &QDialogButtonBox::accepted,
            this, [config, addList, this]() {
        config->getConfigEntriesRef().clear();
        for (int i = 0; i < addList->count(); ++i) {
            auto item = addList->item(i);
            QString path = item->data(Qt::UserRole).toString();
            config->addConfig(ConfigEntry(path));
        }
        config->saveConfig();
        close();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, [this]() { close(); });
}
