#ifndef DEVICEINFOFORM_H
#define DEVICEINFOFORM_H

#include <QWidget>

namespace Ui
{
    class DeviceInfoForm;
}

class DeviceInfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceInfoForm(QWidget *parent = nullptr);
    ~DeviceInfoForm();

protected:
    // 重写paintEvent以确保样式表能正确应用
    void paintEvent(QPaintEvent *event) override;

private:
    Ui::DeviceInfoForm *ui;
};

#endif // DEVICEINFOFORM_H
