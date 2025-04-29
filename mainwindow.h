#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "component/batterygridwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_menu_clicked();

    void on_btn_about_clicked();

private:
    void initUI();
    void init_sql();
    void cfd_record_action();
    void abnormal_record_action();
    void device_manage_action();
    void user_manage_action();
    void connectBatterySignals(BatteryListForm* battery);

private:
    Ui::MainWindow *ui;
    QMenu *menu_list, *menu_about;
    BatteryGridWidget *batteryGrid;
};
#endif // MAINWINDOW_H 
