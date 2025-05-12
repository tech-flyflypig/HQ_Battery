#include "bms1infoshowform.h"
#include "ui_bms1infoshowform.h"
#include <QDebug>
#include "batterydetailmainform.h"

BMS1InfoShowForm::BMS1InfoShowForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BMS1InfoShowForm)
    , m_currentBattery(nullptr)
{
    ui->setupUi(this);
    
    // 连接BatteryDetailMainForm的返回信号
    if (ui->left_top_widget) {
        connect(ui->left_top_widget, &BatteryDetailMainForm::backToMain, 
                this, &BMS1InfoShowForm::backToMain);
    }
}

BMS1InfoShowForm::~BMS1InfoShowForm()
{
    delete ui;
}

void BMS1InfoShowForm::setBatteryInfo(BatteryListForm *battery)
{
    m_currentBattery = battery;
    
    if (battery) {
        // 获取电池信息
        battery_info info = battery->getBatteryInfo();
        
        qDebug() << "BMS1InfoShowForm: 设置电池信息 - ID:" << info.power_id << ", 位置:" << info.site;
        
        // 传递电池信息到内部组件
        if (ui->left_top_widget) {
            ui->left_top_widget->setBatteryInfo(battery);
        }
        
        // 这里可以添加其他界面元素的更新代码
        // 例如温度曲线、电压曲线等
    }
}
