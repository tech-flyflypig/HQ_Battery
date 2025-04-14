#ifndef LAYOUTBATTERYWIDGET_H
#define LAYOUTBATTERYWIDGET_H

#include <QWidget>
#include "Struct.h"
#include <QMenu>
#include <QLabel>
#include <QVariant>
class LayoutBatteryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit LayoutBatteryWidget(QWidget *parent = nullptr);
    void reviseAction();
    void on_info_show(const QVariant &, battery_info, bool);
    void on_recovery();//备份恢复
    void on_back_up();//备份
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *e) override;

private:
    QMenu *m_contextMenu;
    QLabel *lab;
    bool m_show;
signals:
    void out_data(QVariant, QString type);
};

#endif // LAYOUTBATTERYWIDGET_H
