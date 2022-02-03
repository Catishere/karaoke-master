#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFormLayout>

#include "keypressdialog.h"
#include "stringpairlist.h"

class QPushButton;
class QLabel;

class InputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputDialog(QWidget *parent = nullptr,
                         StringPairList inputs = StringPairList(),
                         QString title = "InputDialog");

    StringPairList getStrings(bool *ok);

private:
    void openKeyInputDialog(QPushButton *button, QString command);
    QList<QPushButton*> fields;
    StringPairList bindings;
};

#endif // INPUTDIALOG_H
