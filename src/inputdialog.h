#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QFormLayout>

typedef QList<QPair<QString, QString>> KeyBindings;

class QLineEdit;
class QLabel;

class InputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit InputDialog(QWidget *parent = nullptr,
                         KeyBindings inputs = KeyBindings());

    static KeyBindings getStrings(QWidget *parent,
                                  KeyBindings inputs,
                                  bool *ok);

private:
    QList<QLineEdit*> fields;
};

#endif // INPUTDIALOG_H
