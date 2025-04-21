#include "BlueGlowWidgetDemo.h"
#include <QApplication>

BlueGlowWidgetDemo::BlueGlowWidgetDemo(QWidget *parent)
    : QWidget(parent)
{
    // 设置窗口标题和尺寸
    setWindowTitle("蓝色发光效果演示");
    resize(600, 500);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);
    
    // 创建发光效果控件
    blueGlowWidget = new BlueGlowWidget(this);
    blueGlowWidget->setTitleText("保存参数");
    
    // 添加两组单选按钮
    QStringList settingOptions = {"过充电压", "过放电压", "输出电源", "内部温度"};
    blueGlowWidget->addRadioGroup("", settingOptions);
    
    QStringList moreOptions = {"环境温度", "均衡管理器"};
    blueGlowWidget->addRadioGroup("", moreOptions);
    
    // 添加获取选项的按钮
    getOptionsButton = new QPushButton("获取当前选项", this);
    connect(getOptionsButton, &QPushButton::clicked, this, &BlueGlowWidgetDemo::onGetSelectedOptions);
    
    // 结果显示标签
    resultLabel = new QLabel(this);
    resultLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    resultLabel->setAlignment(Qt::AlignCenter);
    resultLabel->setMinimumHeight(40);
    
    // 添加控件到主布局
    mainLayout->addWidget(blueGlowWidget);
    mainLayout->addWidget(getOptionsButton);
    mainLayout->addWidget(resultLabel);
    
    // 设置窗口样式
    setStyleSheet("QWidget { background-color: #f0f0f0; }");
}

BlueGlowWidgetDemo::~BlueGlowWidgetDemo()
{
}

void BlueGlowWidgetDemo::onGetSelectedOptions()
{
    QString group1Selection = blueGlowWidget->getSelectedOption(0);
    QString group2Selection = blueGlowWidget->getSelectedOption(1);
    
    QString resultText = QString("选中的选项：组1=%1，组2=%2").arg(group1Selection).arg(group2Selection);
    resultLabel->setText(resultText);
} 