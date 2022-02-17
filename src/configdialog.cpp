#include "configdialog.h"

ConfigDialog::ConfigDialog(QWidget *parent, ConfigController *config)
    : QDialog(parent)
{
    QVBoxLayout *vbox = new QVBoxLayout(this);
    QHBoxLayout *hbox = new QHBoxLayout();
    QVBoxLayout *centerButtons = new QVBoxLayout();
    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok
                                        | QDialogButtonBox::Close);
    auto addList = new ConfigListWidget(this);
    auto trashList = new ConfigListWidget(this);
    addList->setDragDropMode(QAbstractItemView::DragDrop);
    trashList->setDragDropMode(QAbstractItemView::DragDrop);

    for (auto& entry : config->getConfigEntries()) {
        addList->addItem(new QListWidgetItem(QString("%1 [%2]")
                                             .arg(entry.getFullName(),
                                                  entry.getPath())));
    }

    centerButtons->addStretch();
    centerButtons->addWidget(new QPushButton("<",this));
    centerButtons->addWidget(new QPushButton(">",this));
    centerButtons->addStretch();
    hbox->addWidget(addList);
    hbox->addLayout(centerButtons);
    hbox->addWidget(trashList);
    vbox->addLayout(hbox);
    vbox->addWidget(buttons);
    setWindowTitle("Manage configs");
    setLayout(vbox);
}
