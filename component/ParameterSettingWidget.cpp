#include "ParameterSettingWidget.h"
#include <QVBoxLayout>

ParameterSettingWidget::ParameterSettingWidget(QWidget *parent)
    : BlueGlowWidget(parent), currentGroupIndex(0)
{
    // 设置标题
    setTitleText("电池参数设置");
    
    // 添加参数类型选择组
    QStringList parameterTypes = {
        "过充电压", "过放电压", "输出电源", "内部温度",
        "环境温度", "均衡管理器", "电池容量", "通信参数"
    };
    addRadioGroup("参数类型", parameterTypes);
    
    // 添加应用按钮
    applyButton = new QPushButton("应用设置", this);
    applyButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #0066cc;"
        "  color: white;"
        "  border-radius: 4px;"
        "  padding: 6px 12px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "  background-color: #0077ee;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #0055aa;"
        "}"
    );
    
    // 将按钮添加到布局中
    // 注意：我们需要访问父类的私有成员mainLayout，因此在这里创建一个新的布局
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        layout->addWidget(applyButton);
    }
    
    // 连接信号和槽
    // 这需要BlueGlowWidget类添加选择改变的信号，或者其他方式监听选择变化
    // 由于父类BlueGlowWidget没有提供这样的信号，我们可以使用定时器或其他方法
    // 但这里只是示例，实际应用中您可能需要修改BlueGlowWidget添加必要的信号
    connect(applyButton, &QPushButton::clicked, this, &ParameterSettingWidget::onSelectionChanged);
}

QString ParameterSettingWidget::getSelectedParameterType() const
{
    return getSelectedOption(currentGroupIndex);
}

void ParameterSettingWidget::onSelectionChanged()
{
    // 获取当前选择并发送信号
    QString selectedType = getSelectedParameterType();
    emit parameterTypeChanged(selectedType);
} 