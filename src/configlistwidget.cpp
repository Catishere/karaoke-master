#include "configlistwidget.h"

ConfigListWidget::ConfigListWidget(QWidget *parent)
    : QListWidget{parent}
{
}

void ConfigListWidget::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->source() != this) {
        setDragDropMode(DragDropMode::DragDrop);
    } else {
        setDragDropMode(DragDropMode::InternalMove);
    }
}
