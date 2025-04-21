#ifndef BLUEGLOWWIDGETDEMO_H
#define BLUEGLOWWIDGETDEMO_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "BlueGlowWidget.h"

class BlueGlowWidgetDemo : public QWidget
{
    Q_OBJECT

public:
    explicit BlueGlowWidgetDemo(QWidget *parent = nullptr);
    ~BlueGlowWidgetDemo();

private slots:
    void onGetSelectedOptions();

private:
    BlueGlowWidget *blueGlowWidget;
    QPushButton *getOptionsButton;
    QLabel *resultLabel;
};

#endif // BLUEGLOWWIDGETDEMO_H 