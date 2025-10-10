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

    // 保存原始 geometry
    void saveOriginalGeometries();

    // 递归缩放控件
    void scaleWidget(QWidget *widget, double scaleRatio);

    Ui::LeftStatsForm *ui;
    QMap<QWidget*, QRect> m_originalGeometries;
    bool m_geometriesInitialized;
};

#endif // LEFTSTATSFORM_H
