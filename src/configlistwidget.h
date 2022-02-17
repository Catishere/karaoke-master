#ifndef CONFIGLISTWIDGET_H
#define CONFIGLISTWIDGET_H

#include <QListWidget>
#include <QDragEnterEvent>

class ConfigListWidget : public QListWidget
{
    Q_OBJECT
public:
    explicit ConfigListWidget(QWidget *parent = nullptr);

protected:
    void dragMoveEvent(QDragMoveEvent *e) override;
};

#endif // CONFIGLISTWIDGET_H
