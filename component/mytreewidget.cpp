#include "mytreewidget.h"
#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include "Struct.h"
#include <QDebug>
MyTreeWidget::MyTreeWidget( QWidget *parent) : QTreeWidget(parent)
{
    setDragEnabled(true);

}

void MyTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("battery_info"))
    {
        event->setDropAction(Qt::MoveAction);
        event->accept();
    }
    else
    {
        event->ignore();
    }
}
void MyTreeWidget::startDrag(Qt::DropActions /*supportedActions*/)
{
    QTreeWidgetItem *item = currentItem();
    QByteArray itemData;
    battery_info battery = item->data(0, Qt::UserRole).value<battery_info>();
    itemData.append((char *)&battery, sizeof(battery));
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("battery_info", itemData);
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    QPixmap pix;
    if(battery.type == "BMS_1")
    {
        pix.load(":/image/BMS_1.png");
    }
    else
    {
        pix.load(":/image/BMS_2.png");
    }
    drag->setHotSpot(QPoint(pix.width() / 2, pix.height() / 2));
    drag->setPixmap(pix);
    drag->exec(Qt::MoveAction);
//    if (drag->exec(Qt::MoveAction) == Qt::MoveAction)
//        delete takeItem(row(item));
}
