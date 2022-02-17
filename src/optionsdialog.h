#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QObject>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGroupBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QListWidget>

#include "lyricsfetcher.h"
#include "configcontroller.h"
#include "configdialog.h"

class OptionsDialog : public QDialog
{
    Q_OBJECT

private:
    ConfigController *configController;
    QList<LyricsFetcher *> *all_fetchers;
public:
    explicit OptionsDialog(QWidget *parent = nullptr,
                           ConfigController *configCtrl = nullptr,
                           QList<LyricsFetcher *> *all_fetchers = nullptr);
    void openFetcherPriorityDialog();

signals:
    void updateAllowedFetchers();
};

#endif // OPTIONSDIALOG_H
