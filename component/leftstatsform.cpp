#include "leftstatsform.h"
#include "ui_leftstatsform.h"
#include "BatteryStats.h"
#include <QDebug>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>

LeftStatsForm::LeftStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftStatsForm)
{
    ui->setupUi(this);

    // 初始化RadioButton为只读状态
    initializeRadioButtons();

    // 连接统计数据变化信号
    connect(BatteryStats::instance(), &BatteryStats::statsChanged, this, &LeftStatsForm::updateStats);

    // 初始化统计显示
    updateStats();
}

LeftStatsForm::~LeftStatsForm()
{
    delete ui;
}

void LeftStatsForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    adjustTopMargins();
}

void LeftStatsForm::adjustTopMargins()
{
    // 基准高度（UI 文件中定义的）
    const double kBaseHeight = 951.0;
    // 基准值
    const int kBaseProtectMargin = 75;
    const int kBaseFaultMargin = 70;
    const int kBaseMainSpacing = 19;  // 主布局的 spacing
    const int kBaseBatteryNumberSpacing = 23;  // 电池数量内部 VBoxLayout 的 spacing
    const int kBaseBatteryGridTopMargin = 8;  // 电池数量内部 GridLayout 的 topMargin
    const int kBaseFormVerticalSpacing = 6;  // FormLayout 默认竖向间距
    const int kBaseProtectHorizontalSpacing = 26;  // 保护状态的 horizontalSpacing
    const int kBaseFaultHorizontalSpacing = 28;  // 故障报警的 horizontalSpacing

    // 当前高度
    double currentHeight = this->height();

    // 计算缩放比例
    double ratio = currentHeight / kBaseHeight;

    // 限制缩放范围，避免过小或过大
    if (ratio < 0.8) ratio = 0.8;
    if (ratio > 1.5) ratio = 1.5;

    // 动态计算新的间距值
    int newProtectMargin = static_cast<int>(kBaseProtectMargin * ratio);
    int newFaultMargin = static_cast<int>(kBaseFaultMargin * ratio);
    int newMainSpacing = static_cast<int>(kBaseMainSpacing * ratio);
    int newBatteryNumberSpacing = static_cast<int>(kBaseBatteryNumberSpacing * ratio);
    int newBatteryGridTopMargin = static_cast<int>(kBaseBatteryGridTopMargin * ratio);
    int newFormVerticalSpacing = static_cast<int>(kBaseFormVerticalSpacing * ratio);
    int newProtectHorizontalSpacing = static_cast<int>(kBaseProtectHorizontalSpacing * ratio);
    int newFaultHorizontalSpacing = static_cast<int>(kBaseFaultHorizontalSpacing * ratio);

    // 1. 调整主布局的 spacing（三个容器之间的间距）
    if (QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(this->layout()))
    {
        mainLayout->setSpacing(newMainSpacing);
    }

    // 2. 调整电池数量容器内部的布局
    if (QVBoxLayout *batteryLayout = ui->battery_number->findChild<QVBoxLayout*>("verticalLayout"))
    {
        batteryLayout->setSpacing(newBatteryNumberSpacing);

        // 调整内部 GridLayout 的 topMargin
        if (QGridLayout *gridLayout = batteryLayout->findChild<QGridLayout*>("gridLayout"))
        {
            QMargins margins = gridLayout->contentsMargins();
            margins.setTop(newBatteryGridTopMargin);
            gridLayout->setContentsMargins(margins);
        }
    }

    // 3. 调整保护状态容器的布局
    if (QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->protect_status->layout()))
    {
        QMargins margins = gridLayout->contentsMargins();
        margins.setTop(newProtectMargin);
        gridLayout->setContentsMargins(margins);

        // 获取内部的 FormLayout 并调整间距
        if (QFormLayout *formLayout = ui->protect_status->findChild<QFormLayout*>("formLayout"))
        {
            formLayout->setVerticalSpacing(newFormVerticalSpacing);
            formLayout->setHorizontalSpacing(newProtectHorizontalSpacing);
        }
    }

    // 4. 调整故障报警容器的布局
    if (QGridLayout *gridLayout = qobject_cast<QGridLayout*>(ui->fault_alarm->layout()))
    {
        QMargins margins = gridLayout->contentsMargins();
        margins.setTop(newFaultMargin);
        gridLayout->setContentsMargins(margins);

        // 获取内部的 FormLayout 并调整间距
        if (QFormLayout *formLayout = ui->fault_alarm->findChild<QFormLayout*>("formLayout_2"))
        {
            formLayout->setVerticalSpacing(newFormVerticalSpacing);
            formLayout->setHorizontalSpacing(newFaultHorizontalSpacing);
        }
    }
}

// 初始化RadioButton为只读状态
void LeftStatsForm::initializeRadioButtons()
{
    // 创建RadioButton列表
    QList<QRadioButton *> allRadioButtons;

    // 故障报警部分的RadioButton
    allRadioButtons << ui->rbtn_voltage_alarm   // 电压报警
                    << ui->rbtn_current_alarm   // 电流报警
                    << ui->rbtn_temperature_alarm; // 温度告警

    // 保护状态部分的RadioButton
    allRadioButtons << ui->rbtn_short_circuit // 输出短路
                    << ui->rbtn_tempeture_protect // 温度保护
                    << ui->rbtn_over_voltage_and_discharge; // 过压过放

    // 批量设置所有RadioButton为只读
    for (QRadioButton *radioButton : allRadioButtons)
    {
        // 使鼠标事件透明，阻止用户点击
        radioButton->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        radioButton->setFocusPolicy(Qt::NoFocus); // 防止键盘焦点
    }
}

void LeftStatsForm::updateStats()
{
    BatteryStats *stats = BatteryStats::instance();

    // 输出当前统计信息
    // qDebug() << "更新左侧统计信息 - 总数:" << stats->getTotalBatteryCount()
    //          << "运行:" << stats->getRunningBatteryCount()
    //          << "停止:" << stats->getStoppedBatteryCount()
    //          << "故障:" << stats->getFaultBatteryCount();

    // 更新基本统计
    ui->label_total_num->setText(QString::number(stats->getTotalBatteryCount()));
    ui->verticalSlider_running->setRange(0, stats->getTotalBatteryCount());
    ui->verticalSlider_stop->setRange(0, stats->getTotalBatteryCount());
    ui->verticalSlider_fault->setRange(0, stats->getTotalBatteryCount());

    ui->label_runing_num->setText(QString::number(stats->getRunningBatteryCount()));
    ui->verticalSlider_running->setValue(stats->getRunningBatteryCount());
    ui->label_stop_num->setText(QString::number(stats->getStoppedBatteryCount()));
    ui->verticalSlider_stop->setValue(stats->getStoppedBatteryCount());
    ui->label_fault_num->setText(QString::number(stats->getFaultBatteryCount()));
    ui->verticalSlider_fault->setValue(stats->getFaultBatteryCount());

    // 更新保护状态统计
    // 过压过放
    int over_voltage_discharge_protect = stats->getSingleOvervoltageCount() + stats->getSingleUndervoltageCount() +
                                         stats->getTotalOvervoltageCount() + stats->getTotalUndervoltageCount();
    ui->label_over_voltage_discharge_protect->setText(QString::number(over_voltage_discharge_protect));

    // 温度保护
    int tempeture_protect = stats->getCellChargingHighTempCount() + stats->getCellDischargingHighTempCount() +
                            stats->getCellChargingLowTempCount() + stats->getCellDischargingLowTempCount() +
                            stats->getPcbHighTempCount() + stats->getPcbLowTempCount() +
                            stats->getPowerEquipProtectCount();
    ui->label_tempeture_protect->setText(QString::number(tempeture_protect));

    // 输出短路
    int short_circuit = stats->getShortCircuitCount();
    ui->label_shortCircuitProtect_num->setText(QString::number(short_circuit));

    // 更新告警统计
    // 电压告警
    int voltage_alarm = stats->getSingleHighVoltageAlarmCount() + stats->getSingleLowVoltageAlarmCount() +
                        stats->getTotalHighVoltageAlarmCount() + stats->getTotalLowVoltageAlarmCount();
    ui->label_voltage_alarm->setText(QString::number(voltage_alarm));

    // 温度告警
    int temperature_alarm = stats->getCellChargingHighTempAlarmCount() + stats->getCellDischargingHighTempAlarmCount() +
                            stats->getCellChargingLowTempAlarmCount() + stats->getCellDischargingLowTempAlarmCount() +
                            stats->getPcbHighTempAlarmCount() + stats->getPcbLowTempAlarmCount() +
                            stats->getPowerEquipHighTempAlarmCount();
    ui->label_temperature_alarm->setText(QString::number(temperature_alarm));

    // 电流告警
    int current_alarm = stats->getChargingOvercurrentAlarmCount() + stats->getDischargingOvercurrentAlarmCount();
    ui->label_current_alarm->setText(QString::number(current_alarm));

    // 更新RadioButton选中状态
    ui->rbtn_voltage_alarm->setChecked(voltage_alarm > 0);
    ui->rbtn_current_alarm->setChecked(current_alarm > 0);
    ui->rbtn_temperature_alarm->setChecked(temperature_alarm > 0);

    ui->rbtn_over_voltage_and_discharge->setChecked(over_voltage_discharge_protect > 0);
    ui->rbtn_tempeture_protect->setChecked(tempeture_protect > 0);
    ui->rbtn_short_circuit->setChecked(short_circuit > 0);

    // 告警标签颜色设置 (有告警时为黄色，无告警时为蓝色)
    QString alarmActiveStyle = "QLabel{color: rgb(246, 233, 47);}";
    QString alarmInactiveStyle = "QLabel{color: rgb(0, 194, 223);}";

    // 保护状态标签颜色设置 (有保护时为黄色，无保护时为白色)
    QString protectActiveStyle = "QLabel{color: rgb(246, 233, 47);}";
    QString protectInactiveStyle = "QLabel{color: rgb(255, 255, 255);}";

    // 电池图标样式设置
    QString batteryActiveStyle = ".QWidget{border-image: url(:/image/电源_A.png);background:transparent;}";
    QString batteryInactiveStyle = ".QWidget{border-image: url(:/image/电源.png);background:transparent;}";

    // 设置告警标签颜色
    ui->label_voltage_alarm->setStyleSheet(voltage_alarm > 0 ? alarmActiveStyle : alarmInactiveStyle);
    ui->label_current_alarm->setStyleSheet(current_alarm > 0 ? alarmActiveStyle : alarmInactiveStyle);
    ui->label_temperature_alarm->setStyleSheet(temperature_alarm > 0 ? alarmActiveStyle : alarmInactiveStyle);

    // 设置保护状态标签颜色
    ui->label_over_voltage_discharge_protect->setStyleSheet(over_voltage_discharge_protect > 0 ? protectActiveStyle : protectInactiveStyle);
    ui->label_tempeture_protect->setStyleSheet(tempeture_protect > 0 ? protectActiveStyle : protectInactiveStyle);
    ui->label_shortCircuitProtect_num->setStyleSheet(short_circuit > 0 ? protectActiveStyle : protectInactiveStyle);

    // 设置保护状态对应的电池图标样式
    ui->widget_battery_bak->setStyleSheet(over_voltage_discharge_protect > 0 ? batteryActiveStyle : batteryInactiveStyle);
    ui->widget_battery_bak_2->setStyleSheet(tempeture_protect > 0 ? batteryActiveStyle : batteryInactiveStyle);
    ui->widget_battery_bak_3->setStyleSheet(short_circuit > 0 ? batteryActiveStyle : batteryInactiveStyle);
}
