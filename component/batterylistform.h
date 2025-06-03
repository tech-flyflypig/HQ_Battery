#ifndef BATTERYLISTFORM_H
#define BATTERYLISTFORM_H

#include <QWidget>
#include <memory>
#include "../protocols/communicationworker.h"
#include "../protocols/communicationworkerfactory.h"
#include "../utils/Struct.h"

namespace Ui
{
    class BatteryListForm;
}

class BatteryListForm : public QWidget, public std::enable_shared_from_this<BatteryListForm>
{
    Q_OBJECT

public:
    // 电池充放电状态枚举
    enum BatteryChargeState {
        Unknown = -1,    // 未知状态
        Discharging = 0, // 放电状态
        Charging = 1,    // 充电状态
        Idle = 2         // 空闲状态
    };

    explicit BatteryListForm(QWidget *parent = nullptr);
    ~BatteryListForm();
    void setSelected(bool selected);

    // 设置电池信息
    void setBatteryInfo(const battery_info &info);
    battery_info getBatteryInfo() const;

    // 开始/停止通信
    void startCommunication();
    void stopCommunication();
    
    // 获取最新的电池数据
    BMS_1 getLastData() const { return m_lastData; }
    
    // 获取通信接口
    CommunicationWorker* getCommunicationWorker() const { return m_communicationWorker; }

    // 获取当前充放电状态
    BatteryChargeState getChargeState() const { return m_chargeState; }

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
    // 初始化通信
    void initCommunication();
    // 更新UI显示
    void updateDisplay(const BMS_1 &data);
    // 根据系统状态确定充放电状态
    BatteryChargeState determineChargeState(uint16_t systemStatus) const;
    // 记录充放电状态变化到数据库
    void recordChargeStateChange(BatteryChargeState newState);

signals:
    void selectedChanged(bool);
    void clicked(BatteryListForm *);
    void doubleclicked(BatteryListForm *);
    void communicationError(BatteryListForm *, const QString &);
    void communicationTimeout(BatteryListForm *);
    void dataReceived(BatteryListForm *, const BMS_1 &);
    void chargeStateChanged(BatteryListForm *, BatteryChargeState); // 充放电状态变化信号

private slots:
    void onBatteryDataReceived(const BMS_1 &data);
    void onCommunicationError(const QString &errorMessage);
    void onCommunicationTimeout();

private:
    Ui::BatteryListForm *ui;
    bool m_selected;
    CommunicationWorker *m_communicationWorker;
    battery_info m_batteryInfo;
    BMS_1 m_lastData;
    bool m_isRunning;
    CommunicationType m_communicationType;
    BatteryChargeState m_chargeState; // 当前充放电状态
    uint16_t m_lastSystemStatus; // 上一次的系统状态值
};

#endif // BATTERYLISTFORM_H
