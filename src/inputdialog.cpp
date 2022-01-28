#include "inputdialog.h"

InputDialog::InputDialog(QWidget *parent,  StringPairList inputs,
                         QString title)
    : QDialog(parent)
{
    this->bindings = inputs;
    this->setWindowTitle(title);
    QFormLayout *lytMain = new QFormLayout(this);
    for (auto &item : inputs)
    {
        QLabel *tLabel = new QLabel(item.first + ':', this);
        QPushButton *button = new QPushButton(this);
        connect(button, &QPushButton::pressed,
                this, [=](){
            button->setDisabled(true);
            getKeyInput(button, item.first);
            button->setDisabled(false);
        });
        button->setText(item.second);
        lytMain->addRow(tLabel, button);

        fields << button;
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &InputDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &InputDialog::reject);

    setLayout(lytMain);
}

StringPairList InputDialog::getStrings(bool *ok)
{
    StringPairList list;

    const int ret = exec();
    *ok = !!ret;

    if (ret) {
        for (int i = 0; i < fields.size(); i++) {
            list.append({bindings[i].first, fields[i]->text()});
        }
    }

    deleteLater();

    return list;
}

void InputDialog::getKeyInput(QPushButton *button, QString command)
{
    KeyPressDialog *kpd = new KeyPressDialog(this, button->text(), command);
    connect(kpd, &KeyPressDialog::keyPressed,
            this, [=](QString key) {
        kpd->close();
        if (key != "esc")
            button->setText(key);
    });
    kpd->exec();
}
