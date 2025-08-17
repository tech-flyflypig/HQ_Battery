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

# CONFIG(release, debug|release){
# LIBS += -L$$PWD/lib -llwdesigner
# }else
# {
# LIBS += -L$$PWD/lib -llwdesignerd
# }

# 默认情况下，在 Release 模式下禁用 qDebug 输出
# CONFIG(release, debug|release) 表示：
# 如果当前配置是 Release 模式（第一个参数），并且它不是 Debug 模式（在 debug|release 模式中），则执行下一行
# 这确保了只有在 Release 构建时才添加 QT_NO_DEBUG_OUTPUT
CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# 如果你想在所有构建模式下都禁用 qDebug 输出，可以直接这样：
# DEFINES += QT_NO_DEBUG_OUTPUT


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
    component/chargeanddischargerecordform.cpp \
    component/exceptionform.cpp \
    component/exceptionrecordform.cpp \
    component/leftstatsform.cpp \
    component/logininform.cpp \
    component/mytreewidget.cpp \
    component/queryform.cpp \
    component/revisepowerform.cpp \
    component/rightstatsform.cpp \
    component/batterychartwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    protocols/bms1battery.cpp \
    protocols/modbustcpworker.cpp \
    protocols/protocol.cpp \
    protocols/serialworker.cpp \
    utils/BatteryStats.cpp \
    utils/ModbusHelper.cpp \
    utils/myapp.cpp \
    utils/RecordManager.cpp

HEADERS += \
    component/addpowerform.h \
    component/adduser.h \
    component/adduserform.h \
    component/batterylistform.h \
    component/batterygridwidget.h \
    component/bms1infoshowform.h \
    component/cfdrecordform.h \
    component/chargeanddischargerecordform.h \
    component/exceptionform.h \
    component/exceptionrecordform.h \
    component/leftstatsform.h \
    component/logininform.h \
    component/mytreewidget.h \
    component/queryform.h \
    component/revisepowerform.h \
    component/rightstatsform.h \
    component/batterychartwidget.h \
    mainwindow.h \
    protocols/batteryinterface.h \
    protocols/batteryinterfacefactory.h \
    protocols/bms1battery.h \
    protocols/communicationworker.h \
    protocols/communicationworkerfactory.h \
    protocols/modbustcpworker.h \
    protocols/protocol.h \
    protocols/serialworker.h \
    utils/BatteryStats.h \
    utils/ModbusHelper.h \
    utils/RecordManager.h \
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
    component/chargeanddischargerecordform.ui \
    component/exceptionform.ui \
    component/exceptionrecordform.ui \
    component/leftstatsform.ui \
    component/logininform.ui \
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
