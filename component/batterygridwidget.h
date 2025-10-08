#ifndef BATTERYGRIDWIDGET_H
#define BATTERYGRIDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <memory>
#include "batterylistform.h"

class BatteryGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryGridWidget(QWidget *parent = nullptr);
    ~BatteryGridWidget();

    // 设置网格布局的行列数
    void setGridSize(int rows, int cols);
    
    // 设置总电池数量
    void setTotalItems(int count);
    
    // 刷新布局
    void refreshLayout();
    
    // 获取当前选中的电池
    BatteryListForm* getSelectedBattery() const;
    
    // 获取所有电池组件
    QList<BatteryListForm*> getBatteryWidgets() const;
    
    // 设置底部空间
    void setBottomMargin(int margin);
    
    // 设置是否自动隐藏分页控件
    void setAutoHidePagination(bool autoHide);

signals:
    // 当电池选择改变时发出的信号
    void batterySelected(BatteryListForm* battery);
    // 当电池被双击时发出的信号
    void batteryDoubleClicked(BatteryListForm* battery);

protected:
    void paintEvent(QPaintEvent *);
private slots:
    // 处理页面切换
    void onPrevPageClicked();
    void onNextPageClicked();
    
    // 处理电池选择
    void onBatteryClicked(BatteryListForm* battery);
    // 处理电池双击
    void onBatteryDoubleClicked(BatteryListForm* battery);

private:
    // 更新当前页面显示
    void updatePage();
    
    // 配置UI
    void setupUI();
    
    // 更新分页控件显示状态
    void updatePaginationVisibility();

private:
    QWidget *containerWidget;
    QGridLayout *gridLayout;
    QVBoxLayout *mainLayout;
    QHBoxLayout *pageControlLayout;
    QWidget *pageControlWidget;
    
    QPushButton *prevButton;
    QPushButton *nextButton;
    QLabel *pageInfoLabel;
    
    QList<std::shared_ptr<BatteryListForm>> batteryWidgets;
    
    int rows;
    int cols;
    int itemsPerPage;
    int totalItems;
    int currentPage;
    int totalPages;
    int bottomMargin;
    
    bool autoHidePagination;
    std::shared_ptr<BatteryListForm> selectedBattery;
};

#endif // BATTERYGRIDWIDGET_H
