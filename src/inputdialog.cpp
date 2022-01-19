#include "inputdialog.h"

InputDialog::InputDialog(QWidget *parent,  KeyBindings inputs)
    : QDialog(parent)
{
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

KeyBindings InputDialog::getStrings(QWidget *parent,
                                    KeyBindings inputs,
                                    bool *ok)
{
    InputDialog *dialog = new InputDialog(parent, inputs);

    KeyBindings list;

    const int ret = dialog->exec();
    *ok = !!ret;

    if (ret) {
        for (int i = 0; i < dialog->fields.size(); i++) {
            list.append({inputs[i].first, dialog->fields[i]->text()});
        }
    }


    dialog->deleteLater();

    return list;
}
