#ifndef BLUEGLOWWIDGET_H
#define BLUEGLOWWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPainter>
#include <QStyleOption>

class BlueGlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BlueGlowWidget(QWidget *parent = nullptr);
    ~BlueGlowWidget();

    // 添加单选按钮组
    void addRadioGroup(const QString &groupTitle, const QStringList &options);
    
    // 设置标题
    void setTitleText(const QString &title);
    
    // 获取选择的选项
    QString getSelectedOption(int groupIndex) const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QList<QGroupBox*> radioGroups;
    QList<QList<QRadioButton*>> radioButtons;
    
    // 设置样式
    void setupStyle();
    
    // 创建内发光效果
    void createInnerGlowEffect();
};

#endif // BLUEGLOWWIDGET_H 