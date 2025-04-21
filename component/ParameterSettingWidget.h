#ifndef PARAMETERSETTINGWIDGET_H
#define PARAMETERSETTINGWIDGET_H

#include "BlueGlowWidget.h"
#include <QPushButton>

// 参数设置控件，用于BMS系统各项参数的设置
class ParameterSettingWidget : public BlueGlowWidget
{
    Q_OBJECT

public:
    explicit ParameterSettingWidget(QWidget *parent = nullptr);
    
    // 获取当前选择的参数类型
    QString getSelectedParameterType() const;
    
signals:
    // 当参数类型改变时发出信号
    void parameterTypeChanged(const QString &paramType);
    
private slots:
    // 当选择改变时处理
    void onSelectionChanged();
    
private:
    QPushButton *applyButton;
    int currentGroupIndex;
};

#endif // PARAMETERSETTINGWIDGET_H 