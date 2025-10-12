#include "batterygridwidget.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>

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
    // 使用shared_ptr，不需要手动清理
    batteryWidgets.clear();
}

void BatteryGridWidget::setupUI()
{
    //this->setStyleSheet("background-color: rgb(85, 170, 255);");
    // 创建主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // 创建容器和网格布局
    containerWidget = new QWidget(this);
    gridLayout = new QGridLayout(containerWidget);
    // 移除固定最小尺寸，改为动态计算
    // containerWidget->setMinimumSize(1452, 932);
    // 调整网格间距，使电池显示更紧凑
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(30, 40, 30, 10);

    // 初始化容器最小尺寸
    updateContainerMinimumSize();

    // 设置网格布局的对齐方式为左上角对齐，而不是默认的居中对齐
    gridLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

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

    // 添加布局到主布局，确保所有内容都左上角对齐
    mainLayout->addWidget(containerWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    mainLayout->addWidget(pageControlWidget, 0, Qt::AlignLeft | Qt::AlignTop);
    mainLayout->addStretch(); // 添加弹性空间，确保内容靠上


    // 默认添加底部空白区域
    mainLayout->addSpacing(bottomMargin);

    // 连接信号
    connect(prevButton, &QPushButton::clicked, this, &BatteryGridWidget::onPrevPageClicked);
    connect(nextButton, &QPushButton::clicked, this, &BatteryGridWidget::onNextPageClicked);

    // 初始化按钮状态
    prevButton->setEnabled(false);
    nextButton->setEnabled(false);
}

void BatteryGridWidget::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void BatteryGridWidget::setGridSize(int rows, int cols)
{
    this->rows = rows;
    this->cols = cols;
    this->itemsPerPage = rows * cols;

    // 更新容器最小尺寸
    updateContainerMinimumSize();

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
    batteryWidgets.clear();
    selectedBattery = nullptr;

    // 创建新控件
    totalItems = count;
    for (int i = 0; i < totalItems; ++i)
    {
        auto batteryPtr = BatteryListForm::create(containerWidget);

        // 设置电池组件的固定大小，防止变得过大
        batteryPtr->setFixedSize(220, 110); // 设置更紧凑的宽度和高度

        batteryPtr->setVisible(false);
        batteryWidgets.append(batteryPtr);

        // 连接点击信号
        connect(batteryPtr.get(), &BatteryListForm::clicked, this, [this, ptr = batteryPtr.get()]()
        {
            onBatteryClicked(ptr);
        });

        // 连接双击信号
        connect(batteryPtr.get(), &BatteryListForm::doubleclicked, this, [this, ptr = batteryPtr.get()](BatteryListForm * battery)
        {
            onBatteryDoubleClicked(ptr);
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
    return selectedBattery ? selectedBattery.get() : nullptr;
}

QList<BatteryListForm *> BatteryGridWidget::getBatteryWidgets() const
{
    QList<BatteryListForm *> result;
    for (const auto &ptr : batteryWidgets)
    {
        result.append(ptr.get());
    }
    return result;
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

void BatteryGridWidget::updateContainerMinimumSize()
{
    // 每个电池项的尺寸（与 setFixedSize 保持一致）
    const int kBatteryItemWidth = 220;
    const int kBatteryItemHeight = 110;

    // 网格布局的参数（setupUI 中设置）
    const int kSpacing = 10;  // gridLayout->setSpacing(10)
    const int kLeftMargin = 30;
    const int kTopMargin = 40;
    const int kRightMargin = 30;
    const int kBottomMargin = 10;

    // 根据网格大小动态计算容器最小尺寸
    int minWidth = kLeftMargin + (kBatteryItemWidth * cols) + (kSpacing * (cols - 1)) + kRightMargin;
    int minHeight = kTopMargin + (kBatteryItemHeight * rows) + (kSpacing * (rows - 1)) + kBottomMargin;

    // 限制最大最小尺寸，避免超出屏幕
    const int kMaxMinWidth = 1200;   // 原来的固定宽度上限
    const int kMaxMinHeight = 848;   // 原来的固定高度上限

    if (minWidth > kMaxMinWidth)
    {
        minWidth = kMaxMinWidth;
    }
    if (minHeight > kMaxMinHeight)
    {
        minHeight = kMaxMinHeight;
    }

    // 设置容器的最小尺寸
    containerWidget->setMinimumSize(minWidth, minHeight);

    // qDebug() << "BatteryGridWidget::updateContainerMinimumSize - rows:" << rows
    //          << "cols:" << cols
    //          << "minWidth:" << minWidth
    //          << "minHeight:" << minHeight;
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
    for (const auto &widget : batteryWidgets)
    {
        if (widget.get() != battery)
        {
            widget->setSelected(false);
        }
    }

    // 找到对应的shared_ptr
    for (const auto &ptr : batteryWidgets)
    {
        if (ptr.get() == battery)
        {
            selectedBattery = ptr;
            break;
        }
    }

    // 发送信号
    emit batterySelected(battery);
}

void BatteryGridWidget::onBatteryDoubleClicked(BatteryListForm *battery)
{
    // 先确保电池被选中
    onBatteryClicked(battery);

    // 发送双击信号
    emit batteryDoubleClicked(battery);
}

void BatteryGridWidget::updatePage()
{
    // 先清除网格布局中的所有组件
    QLayoutItem *child;
    while ((child = gridLayout->takeAt(0)) != nullptr)
    {
        delete child;
    }

    // 显示当前页面的控件
    int startIdx = currentPage * itemsPerPage;
    int itemsOnPage = qMin(itemsPerPage, totalItems - startIdx);

    // 简单地从左到右、从上到下布局
    for (int i = 0; i < itemsOnPage; ++i)
    {
        int row = i / cols;
        int col = i % cols;

        auto widget = batteryWidgets[startIdx + i].get();

        // 添加到布局，并指定左上角对齐方式
        if (widget)
        {
            widget->setParent(containerWidget);
            widget->setVisible(true);
            gridLayout->addWidget(widget, row, col, 1, 1, Qt::AlignLeft | Qt::AlignTop);
        }
    }

    // 更新页面信息
    pageInfoLabel->setText(QString("第 %1/%2 页").arg(currentPage + 1).arg(totalPages));

    // 更新按钮状态
    prevButton->setEnabled(currentPage > 0);
    nextButton->setEnabled(currentPage < totalPages - 1);
}
