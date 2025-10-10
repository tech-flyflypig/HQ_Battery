﻿#include "leftstatsform.h"
#include "ui_leftstatsform.h"
#include "BatteryStats.h"
#include <QDebug>
#include <QResizeEvent>
#include <functional>
#include <algorithm>

LeftStatsForm::LeftStatsForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftStatsForm)
    , m_geometriesInitialized(false)
{
    ui->setupUi(this);

    // 初始化RadioButton为只读状态
    initializeRadioButtons();

    // 连接统计数据变化信号
    connect(BatteryStats::instance(), &BatteryStats::statsChanged, this, &LeftStatsForm::updateStats);

    // 初始化统计显示
    updateStats(); // 确保立即更新统计数据
}

LeftStatsForm::~LeftStatsForm()
{
    delete ui;
}

void LeftStatsForm::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 首次运行时保存原始 geometry
    if (!m_geometriesInitialized) {
        saveOriginalGeometries();
    }

    int newWidth = event->size().width();
    int newHeight = event->size().height();
    const double kOriginalWidth = 247.0;
    const double kOriginalHeight = 951.0;

    // 分别计算宽度和高度的缩放比例
    double widthRatio = static_cast<double>(newWidth) / kOriginalWidth;
    double heightRatio = static_cast<double>(newHeight) / kOriginalHeight;

    // 取较小的缩放比例，确保内容不会超出容器
    double scaleRatio = std::min(widthRatio, heightRatio);

    // 限制最小和最大缩放比例
    if (scaleRatio < 0.7) {
        scaleRatio = 0.7;
    }
    if (scaleRatio > 1.2) {  // 降低最大缩放比例，避免过大
        scaleRatio = 1.2;
    }

    // 验证缩放后不超出容器高度
    double scaledHeight = kOriginalHeight * scaleRatio;
    if (scaledHeight > newHeight) {
        // 如果超出，重新计算缩放比例
        scaleRatio = static_cast<double>(newHeight) / kOriginalHeight;
        if (scaleRatio < 0.7) {
            scaleRatio = 0.7;
        }
    }

    // 递归缩放所有保存的控件
    for (auto it = m_originalGeometries.constBegin(); it != m_originalGeometries.constEnd(); ++it) {
        QWidget *widget = it.key();
        const QRect &originalGeom = it.value();

        widget->setGeometry(
            static_cast<int>(originalGeom.x() * scaleRatio),
            static_cast<int>(originalGeom.y() * scaleRatio),
            static_cast<int>(originalGeom.width() * scaleRatio),
            static_cast<int>(originalGeom.height() * scaleRatio)
        );
    }
}

void LeftStatsForm::saveOriginalGeometries()
{
    if (m_geometriesInitialized) return;

    // 递归函数：保存控件及其所有子控件的 geometry
    std::function<void(QWidget*)> saveRecursive = [&](QWidget *widget) {
        // 保存当前控件的 geometry
        m_originalGeometries[widget] = widget->geometry();

        // 递归处理所有子控件
        for (QObject *child : widget->children()) {
            QWidget *childWidget = qobject_cast<QWidget*>(child);
            if (childWidget) {
                saveRecursive(childWidget);
            }
        }
    };

    // 保存三个主容器及其所有子控件
    saveRecursive(ui->battery_number);
    saveRecursive(ui->protect_status);
    saveRecursive(ui->fault_alarm);

    m_geometriesInitialized = true;
}

void LeftStatsForm::scaleWidget(QWidget *widget, double scaleRatio)
{
    // 此函数暂时不用，保留以备将来扩展
    Q_UNUSED(widget);
    Q_UNUSED(scaleRatio);
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
