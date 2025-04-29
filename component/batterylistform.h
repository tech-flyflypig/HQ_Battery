#ifndef BATTERYLISTFORM_H
#define BATTERYLISTFORM_H

#include <QWidget>

namespace Ui
{
    class BatteryListForm;
}

class BatteryListForm : public QWidget
{
    Q_OBJECT

public:
    explicit BatteryListForm(QWidget *parent = nullptr);
    ~BatteryListForm();
    void setSelected(bool selected);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    // 设置初始样式
    void updateStyle();
signals:
    void selectedChanged(bool);
    void   clicked(BatteryListForm *);
private:
    Ui::BatteryListForm *ui;
    bool m_selected;
};

#endif // BATTERYLISTFORM_H
