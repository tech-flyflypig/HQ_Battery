#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QTreeWidget>
#include <QObject>
#include "Struct.h"
class MyTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    MyTreeWidget( QWidget* parent = 0);

protected:
    void dragMoveEvent(QDragMoveEvent* event) override;
    void startDrag(Qt::DropActions supportedActions) override;

};

#endif // MYTREEWIDGET_H
