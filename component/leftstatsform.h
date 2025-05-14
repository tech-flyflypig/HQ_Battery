#ifndef LEFTSTATSFORM_H
#define LEFTSTATSFORM_H

#include <QWidget>
#include <QRadioButton>

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

private slots:
    void updateStats();

private:
    // 初始化RadioButton为只读状态
    void initializeRadioButtons();
    
    Ui::LeftStatsForm *ui;
};

#endif // LEFTSTATSFORM_H
