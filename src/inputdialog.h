#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>

typedef QList<QPair<QString, QString>> StringPairList;

class QLineEdit;
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
    QList<QLineEdit*> fields;
    StringPairList bindings;
};

#endif // INPUTDIALOG_H
