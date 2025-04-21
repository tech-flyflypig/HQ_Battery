#include "BlueGlowWidget.h"
#include <QApplication>
#include <QGraphicsDropShadowEffect>
#include <QButtonGroup>
#include <QGridLayout>

BlueGlowWidget::BlueGlowWidget(QWidget *parent)
    : QWidget(parent)
{
    // 设置背景透明
    setAttribute(Qt::WA_TranslucentBackground);

    // 主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // 标题
    titleLabel = new QLabel(this);
    QFont titleFont = titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSize(12);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white;");
    mainLayout->addWidget(titleLabel);

    setupStyle();
}

BlueGlowWidget::~BlueGlowWidget()
{
}

void BlueGlowWidget::setupStyle()
{
    // 设置整体样式
    setStyleSheet(
        "BlueGlowWidget {"
        "  background-color: #071b3a;"  // 深蓝色背景
        "  border: 1px solid #0a5aa5;"  // 蓝色边框
        "  border-radius: 8px;"         // 圆角
        "}"

        "QGroupBox {"
        "  border: none;"
        "  color: white;"
        "  font-weight: bold;"
        "  margin-top: 10px;"
        "}"

        "QRadioButton {"
        "  color: white;"
        "  spacing: 8px;"
        "}"

        "QRadioButton::indicator {"
        "  width: 16px;"
        "  height: 16px;"
        "  border-radius: 8px;"
        "}"

        "QRadioButton::indicator:unchecked {"
        "  border: 2px solid #0a5aa5;"
        "  background-color: transparent;"
        "}"

        "QRadioButton::indicator:checked {"
        "  border: 2px solid #0a5aa5;"
        "  background-color: #43b0ff;"  // 选中时的蓝色
        "}"
    );

    // 创建内发光效果
    createInnerGlowEffect();
}

// 创建内发光效果
void BlueGlowWidget::createInnerGlowEffect()
{
    // 移除当前的图形效果（如果有）
    if (graphicsEffect())
    {
        delete graphicsEffect();
    }

    // 使用四个阴影效果组合，实现向内发光
    QWidget *container = new QWidget(this);
    container->setObjectName("glowContainer");
    container->setStyleSheet("background: transparent;");

    // 将原有布局移到容器中
    QLayout *oldLayout = layout();
    container->setLayout(oldLayout);

    // 创建新的布局来容纳阴影容器
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(container);

    // 为容器添加均匀的内发光效果
    QGraphicsDropShadowEffect *glowEffect = new QGraphicsDropShadowEffect(this);
    glowEffect->setBlurRadius(20);  // 较小的模糊半径使发光效果更集中
    glowEffect->setColor(QColor(43, 176, 255, 180));  // 明亮的蓝色，适当的透明度
    glowEffect->setOffset(0, 0);  // 无偏移，实现均匀发光
    container->setGraphicsEffect(glowEffect);

    // 设置容器的边距，使阴影能够向内显示
    container->setContentsMargins(20, 20, 20, 20);
}

void BlueGlowWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BlueGlowWidget::setTitleText(const QString &title)
{
    titleLabel->setText(title);
}

void BlueGlowWidget::addRadioGroup(const QString &groupTitle, const QStringList &options)
{
    // 创建单选按钮组容器
    QGroupBox *groupBox = new QGroupBox(groupTitle, this);
    QVBoxLayout *groupLayout = new QVBoxLayout(groupBox);

    // 创建按钮组以确保互斥选择
    QButtonGroup *buttonGroup = new QButtonGroup(groupBox);

    // 创建一个新的单选按钮列表
    QList<QRadioButton *> radioList;

    // 使用网格布局排列单选按钮
    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    int row = 0;
    int col = 0;
    int maxCols = 2; // 每行最多2个按钮

    for (int i = 0; i < options.size(); ++i)
    {
        QRadioButton *radio = new QRadioButton(options[i], groupBox);
        if (i == 0)
        {
            radio->setChecked(true); // 默认选中第一个
        }

        buttonGroup->addButton(radio, i);
        radioList.append(radio);

        gridLayout->addWidget(radio, row, col);

        col++;
        if (col >= maxCols)
        {
            col = 0;
            row++;
        }
    }

    groupLayout->addLayout(gridLayout);
    radioButtons.append(radioList);
    radioGroups.append(groupBox);
    mainLayout->addWidget(groupBox);
}

QString BlueGlowWidget::getSelectedOption(int groupIndex) const
{
    if (groupIndex < 0 || groupIndex >= radioGroups.size())
    {
        return QString();
    }

    for (QRadioButton *radio : radioButtons[groupIndex])
    {
        if (radio->isChecked())
        {
            return radio->text();
        }
    }

    return QString();
}
