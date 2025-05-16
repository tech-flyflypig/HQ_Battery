#ifndef BATTERYLISTFORM_H
#define BATTERYLISTFORM_H

#include <QWidget>
#include <memory>
#include "../protocols/serialworker.h"
#include "../utils/Struct.h"

namespace Ui
{
    class BatteryListForm;
}

class BatteryListForm : public QWidget, public std::enable_shared_from_this<BatteryListForm>
{
    Q_OBJECT

public:
    explicit BatteryListForm(QWidget *parent = nullptr);
    ~BatteryListForm();
    void setSelected(bool selected);

    // 设置电池信息
    void setBatteryInfo(const battery_info &info);
    battery_info getBatteryInfo() const;

    // 开始/停止串口通信
    void startCommunication();
    void stopCommunication();
    
    // 获取最新的电池数据
    BMS_1 getLastData() const { return m_lastData; }

    // 静态方法创建shared_ptr
    static std::shared_ptr<BatteryListForm> create(QWidget *parent = nullptr) {
        return std::shared_ptr<BatteryListForm>(new BatteryListForm(parent));
    }
    
    // 获取shared_ptr
    std::shared_ptr<BatteryListForm> getSharedPtr() {
        return shared_from_this();
    }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    // 设置初始样式
    void updateStyle();
    // 初始化串口通信
    void initSerialCommunication();
    // 更新UI显示
    void updateDisplay(const BMS_1 &data);

signals:
    void selectedChanged(bool);
    void clicked(BatteryListForm *);
    void doubleclicked(BatteryListForm *);
    void communicationError(BatteryListForm *, const QString &);
    void communicationTimeout(BatteryListForm *);
    void dataReceived(BatteryListForm *, const BMS_1 &);

private slots:
    void onBatteryDataReceived(const BMS_1 &data);
    void onCommunicationError(const QString &errorMessage);
    void onCommunicationTimeout();

private:
    Ui::BatteryListForm *ui;
    bool m_selected;
    SerialWorker *m_serialWorker;
    battery_info m_batteryInfo;
    BMS_1 m_lastData;
    bool m_isRunning;
};

#endif // BATTERYLISTFORM_H
