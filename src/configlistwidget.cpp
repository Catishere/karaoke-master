#include "configlistwidget.h"

ConfigListWidget::ConfigListWidget(QWidget *parent)
    : QListWidget{parent}
{
    setDragDropMode(QAbstractItemView::DragDrop);
    setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
    setDefaultDropAction(Qt::DropAction::MoveAction);
}
