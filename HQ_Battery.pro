QT       += core gui serialport sql xlsx charts network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
msvc {
QMAKE_CFLAGS += /utf-8
QMAKE_CXXFLAGS += /utf-8
}

CONFIG(release, debug|release){
LIBS += -L$$PWD/lib -llwdesigner
}else
{
LIBS += -L$$PWD/lib -llwdesignerd
}



INCLUDEPATH +=\
    component \
    utils \
    protocols


SOURCES += \
    component/addpowerform.cpp \
    component/adduser.cpp \
    component/adduserform.cpp \
    component/batterylistform.cpp \
    component/batterygridwidget.cpp \
    component/bms1infoshowform.cpp \
    component/cfdrecordform.cpp \
    component/exceptionform.cpp \
    component/layoutbatterywidget.cpp \
    component/leftstatsform.cpp \
    component/leftstatsform.cpp \
    component/leftstatsform.cpp \
    component/mytreewidget.cpp \
    component/queryform.cpp \
    component/revisepowerform.cpp \
    component/rightstatsform.cpp \
    component/batterychartwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    protocols/batteryport.cpp \
    protocols/bms1battery.cpp \
    protocols/protocol.cpp \
    protocols/serialworker.cpp \
    utils/BatteryStats.cpp \
    utils/ModbusHelper.cpp \
    utils/myapp.cpp

HEADERS += \
    component/addpowerform.h \
    component/adduser.h \
    component/adduserform.h \
    component/batterylistform.h \
    component/batterygridwidget.h \
    component/bms1infoshowform.h \
    component/cfdrecordform.h \
    component/exceptionform.h \
    component/layoutbatterywidget.h \
    component/leftstatsform.h \
    component/leftstatsform.h \
    component/leftstatsform.h \
    component/mytreewidget.h \
    component/queryform.h \
    component/revisepowerform.h \
    component/rightstatsform.h \
    component/batterychartwidget.h \
    mainwindow.h \
    protocols/battery.h \
    protocols/batteryinterface.h \
    protocols/batteryinterfacefactory.h \
    protocols/batteryport.h \
    protocols/bms1battery.h \
    protocols/protocol.h \
    protocols/serialworker.h \
    utils/BatteryStats.h \
    utils/ModbusHelper.h \
    utils/Struct.h \
    utils/myapp.h \
    config.h

FORMS += \
    component/addpowerform.ui \
    component/adduser.ui \
    component/adduserform.ui \
    component/batterylistform.ui \
    component/bms1infoshowform.ui \
    component/cfdrecordform.ui \
    component/exceptionform.ui \
    component/leftstatsform.ui \
    component/leftstatsform.ui \
    component/queryform.ui \
    component/revisepowerform.ui \
    component/rightstatsform.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target 

RESOURCES += \
    res.qrc

VERSION=1.0.0
