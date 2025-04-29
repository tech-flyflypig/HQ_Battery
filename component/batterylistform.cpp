#include "batterylistform.h"
#include "ui_batterylistform.h"

#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>

BatteryListForm::BatteryListForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BatteryListForm)
    , m_selected(false)
{
    ui->setupUi(this);

    // 设置初始样式
    updateStyle();
}

BatteryListForm::~BatteryListForm()
{
    delete ui;
}

// 设置选中状态
void BatteryListForm::setSelected(bool selected)
{
    if (m_selected != selected)
    {
        m_selected = selected;
        updateStyle();
        emit selectedChanged(m_selected);
    }
}

// 鼠标点击事件处理
void BatteryListForm::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // 设置为选中状态
        setSelected(true);
        // 发出点击信号
        emit clicked(this);
    }
    QWidget::mousePressEvent(event);
}

// 更新组件样式
void BatteryListForm::updateStyle()
{
    if (m_selected)
    {
        // 选中状态样式 - 蓝色边框和浅蓝色背景
        setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景_选中.png);} QLabel{ background-color: transparent;}");
    }
    else
    {
        // 未选中状态样式 - 使用原始背景图
        setStyleSheet("BatteryListForm { border-image: url(:/image/单个电池模块背景.png);}QLabel{ background-color: transparent;}");
    }

    // 强制重绘
    update();
}

// 重写绘制事件，确保样式表被应用
void BatteryListForm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
