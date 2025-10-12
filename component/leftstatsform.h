#ifndef LEFTSTATSFORM_H
#define LEFTSTATSFORM_H

#include <QWidget>
#include <QRadioButton>
#include <QMap>
#include <QRect>

namespace Ui
{
    class LeftStatsForm;
}

class LeftStatsForm : public QWidget
{
    Q_OBJECT

public:
    explicit LeftStatsForm(QWidget *parent = nullptr);
    ~LeftStatsForm();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateStats();

private:
    // 初始化RadioButton为只读状态
    void initializeRadioButtons();

    // 动态调整内部布局的上边距
    void adjustTopMargins();

    Ui::LeftStatsForm *ui;
};

#endif // LEFTSTATSFORM_H
