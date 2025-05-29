#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "component/batterygridwidget.h"
#include "component/bms1infoshowform.h"
#include "component/rightstatsform.h"
#include <QPoint>
#include <QTimer>

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

protected:
    // 事件过滤器，可为指定控件添加自定义事件处理
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void on_btn_menu_clicked();
    void on_btn_about_clicked();

    // 切换到主界面
    void switchToMainView();

    // 处理返回按钮点击
    void onBackButtonClicked();

    void on_btn_max_clicked(bool checked);
    void on_btn_min_clicked();

    void on_btn_close_clicked();
    
    // 更新右侧信息面板
    void updateRightPanel(BatteryListForm *battery);
    
    // 更新当前时间
    void updateCurrentTime();

private:
    void initUI();
    void init_sql();
    void cfd_record_action();
    void abnormal_record_action();
    void device_manage_action();
    void user_manage_action();
    void connectBatterySignals(BatteryListForm *battery);

    // 更新widget_2内容（返回按钮或logo）
    void updateWidget2Content(bool showBackButton);

private:
    Ui::MainWindow *ui;
    QMenu *menu_list, *menu_about;
    BatteryGridWidget *batteryGrid;
    BMS1InfoShowForm *bms1InfoShowForm;

    // 返回按钮和logo标签
    QPushButton *m_backButton;
    
    // 时间更新定时器
    QTimer *m_timeTimer;
    
    // 窗口拖拽相关变量
    bool m_isMoving;
    QPoint m_lastPos;
};
#endif // MAINWINDOW_H 
