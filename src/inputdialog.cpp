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
        QLineEdit *tLine = new QLineEdit(this);
        tLine->setText(item.second);
        lytMain->addRow(tLabel, tLine);

        fields << tLine;
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox
            ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
              Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                   this, &InputDialog::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &InputDialog::reject);
    Q_ASSERT(conn);

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
