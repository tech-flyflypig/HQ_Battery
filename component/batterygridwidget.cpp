#include "batterygridwidget.h"
#include <QDebug>

BatteryGridWidget::BatteryGridWidget(QWidget *parent)
    : QWidget(parent),
      rows(3),
      cols(4),
      itemsPerPage(12),
      totalItems(0),
      currentPage(0),
      totalPages(0),
      bottomMargin(0),
      autoHidePagination(true),
      selectedBattery(nullptr)
{
    setupUI();
}

BatteryGridWidget::~BatteryGridWidget()
{
    // 清理资源
    qDeleteAll(batteryWidgets);
    batteryWidgets.clear();
}

void BatteryGridWidget::setupUI()
{
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建容器和网格布局
    containerWidget = new QWidget(this);
    gridLayout = new QGridLayout(containerWidget);
    gridLayout->setSpacing(20);
    gridLayout->setContentsMargins(20, 20, 20, 20);

    // 创建分页控制
    pageControlWidget = new QWidget(this);
    pageControlLayout = new QHBoxLayout(pageControlWidget);
    pageControlLayout->setContentsMargins(0, 10, 0, 0);
    prevButton = new QPushButton("上一页", pageControlWidget);
    nextButton = new QPushButton("下一页", pageControlWidget);
    pageInfoLabel = new QLabel(pageControlWidget);

    pageControlLayout->addStretch();
    pageControlLayout->addWidget(prevButton);
    pageControlLayout->addWidget(pageInfoLabel);
    pageControlLayout->addWidget(nextButton);
    pageControlLayout->addStretch();

    // 添加布局到主布局
    mainLayout->addSpacing(20);
    mainLayout->addWidget(containerWidget);
    mainLayout->addWidget(pageControlWidget);

    // 默认添加底部空白区域
    mainLayout->addSpacing(bottomMargin);

    // 连接信号
    connect(prevButton, &QPushButton::clicked, this, &BatteryGridWidget::onPrevPageClicked);
    connect(nextButton, &QPushButton::clicked, this, &BatteryGridWidget::onNextPageClicked);

    // 初始化按钮状态
    prevButton->setEnabled(false);
    nextButton->setEnabled(false);
}

void BatteryGridWidget::setGridSize(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    this->itemsPerPage = rows * cols;

    // 重新计算总页数
    if (totalItems > 0)
    {
        totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
        updatePage();
    }
}

void BatteryGridWidget::setTotalItems(int count)
{
    if (count <= 0)
    {
        return;
    }

    // 清理现有控件
    qDeleteAll(batteryWidgets);
    batteryWidgets.clear();
    selectedBattery = nullptr;

    // 创建新控件
    totalItems = count;
    for (int i = 0; i < totalItems; ++i)
    {
        BatteryListForm *batteryWidget = new BatteryListForm(containerWidget);
        batteryWidget->setVisible(false);
        batteryWidgets.append(batteryWidget);

        // 连接点击信号
        connect(batteryWidget, &BatteryListForm::clicked, this, [this, batteryWidget]()
        {
            onBatteryClicked(batteryWidget);
        });
    }

    // 计算总页数
    totalPages = (totalItems + itemsPerPage - 1) / itemsPerPage;
    currentPage = 0;

    // 更新页面
    updatePage();

    // 更新分页控件的可见性
    updatePaginationVisibility();
}

void BatteryGridWidget::refreshLayout()
{
    updatePage();
}

BatteryListForm *BatteryGridWidget::getSelectedBattery() const
{
    return selectedBattery;
}

QList<BatteryListForm *> BatteryGridWidget::getBatteryWidgets() const
{
    return batteryWidgets;
}

void BatteryGridWidget::setBottomMargin(int margin)
{
    // 移除原来的底部间距
    if (mainLayout->count() > 0)
    {
        mainLayout->removeItem(mainLayout->itemAt(mainLayout->count() - 1));
    }

    // 设置新的底部间距
    bottomMargin = margin;
    mainLayout->addSpacing(bottomMargin);
}

void BatteryGridWidget::setAutoHidePagination(bool autoHide)
{
    autoHidePagination = autoHide;
    updatePaginationVisibility();
}

void BatteryGridWidget::updatePaginationVisibility()
{
    if (autoHidePagination)
    {
        // 如果电池数量不足一页，隐藏分页控件
        bool shouldHide = (totalItems <= itemsPerPage);
        pageControlWidget->setVisible(!shouldHide);
    }
    else
    {
        // 始终显示
        pageControlWidget->setVisible(true);
    }
}

void BatteryGridWidget::onPrevPageClicked()
{
    if (currentPage > 0)
    {
        currentPage--;
        updatePage();
    }
}

void BatteryGridWidget::onNextPageClicked()
{
    if (currentPage < totalPages - 1)
    {
        currentPage++;
        updatePage();
    }
}

void BatteryGridWidget::onBatteryClicked(BatteryListForm *battery)
{
    // 清除之前选中的电池
    for (auto widget : batteryWidgets)
    {
        if (widget != battery)
        {
            widget->setSelected(false);
        }
    }

    // 更新当前选中的电池
    selectedBattery = battery;

    // 发送信号
    emit batterySelected(battery);
}

void BatteryGridWidget::updatePage()
{
    // 先清除所有控件
    for (auto widget : batteryWidgets)
    {
        widget->setVisible(false);
        widget->setParent(nullptr);
    }

    // 显示当前页面的控件
    int startIdx = currentPage * itemsPerPage;
    for (int i = 0; i < itemsPerPage && startIdx + i < batteryWidgets.size(); ++i)
    {
        int row = i / cols;
        int col = i % cols;

        BatteryListForm *widget = batteryWidgets[startIdx + i];
        widget->setParent(containerWidget);
        widget->setVisible(true);
        gridLayout->addWidget(widget, row, col);
    }

    // 更新页面信息
    pageInfoLabel->setText(QString("第 %1/%2 页").arg(currentPage + 1).arg(totalPages));

    // 更新按钮状态
    prevButton->setEnabled(currentPage > 0);
    nextButton->setEnabled(currentPage < totalPages - 1);
}
