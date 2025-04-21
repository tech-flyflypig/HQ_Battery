#include <QApplication>
#include "component/BlueGlowWidgetDemo.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // 创建并显示演示窗口
    BlueGlowWidgetDemo demo;
    demo.show();
    
    return a.exec();
} 