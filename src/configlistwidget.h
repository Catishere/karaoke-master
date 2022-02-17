#ifndef CONFIGLISTWIDGET_H
#define CONFIGLISTWIDGET_H

#include <QListWidget>
#include <QDropEvent>

class ConfigListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ConfigListWidget(QWidget *parent = nullptr);
};

#endif // CONFIGLISTWIDGET_H
