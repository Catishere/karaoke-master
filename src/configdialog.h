#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QPushButton>

#include "configcontroller.h"
#include "configlistwidget.h"

class ConfigDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConfigDialog(QWidget *parent = nullptr,
                          ConfigController *config = nullptr);

signals:

};

#endif // CONFIGDIALOG_H
