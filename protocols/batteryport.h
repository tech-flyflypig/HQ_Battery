#ifndef BATTERYPORT_H
#define BATTERYPORT_H
 
#include <QWidget>
#include <QMenu>
#include <QLabel>
#include "battery.h"
#include "serialworker.h"
#include "Struct.h"
 
 class BatteryPort : public QWidget
 {
     Q_OBJECT
 public:
     explicit BatteryPort(const battery_info batteryInfo, QWidget *parent = nullptr);
     battery_info getBatteryInfo() { return m_batteryInfo; }
 
 private:
     Battery* m_battery;         // 电池UI组件
     SerialWorker* m_serial;     // 串口工作器
     battery_info m_batteryInfo;
     QPoint clickPos;
     QMenu* m_contextMenu;
     QVariant m_data;
 
     void initThread();
     void initUI();
     void initContextMenu();
     void delete_action();
 
 protected:
     void mousePressEvent(QMouseEvent* e) override;
     void mouseMoveEvent(QMouseEvent* e) override;
     void mouseDoubleClickEvent(QMouseEvent* event) override;
     void enterEvent(QEvent* e) override;
     void leaveEvent(QEvent* e) override;
     void contextMenuEvent(QContextMenuEvent* event) override;
 
 signals:
     void mousedoubleclick();
     void sig_show(bool);
     void out_data(QVariant, QString);
     void show_data(QVariant, battery_info, bool);
     void sig_start_stop(bool);
 };
 
 #endif // BATTERYPORT_H