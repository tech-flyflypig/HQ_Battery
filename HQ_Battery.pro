QT       += core gui serialport sql

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
    component/layoutbatterywidget.cpp \
    component/mytreewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    protocols/batteryport.cpp \
    protocols/bms1battery.cpp \
    protocols/protocol.cpp \
    protocols/serialworker.cpp \
    utils/ModbusHelper.cpp \
    utils/myapp.cpp

HEADERS += \
    component/layoutbatterywidget.h \
    component/mytreewidget.h \
    mainwindow.h \
    protocols/battery.h \
    protocols/batteryinterface.h \
    protocols/batteryinterfacefactory.h \
    protocols/batteryport.h \
    protocols/bms1battery.h \
    protocols/protocol.h \
    protocols/serialworker.h \
    utils/ModbusHelper.h \
    utils/Struct.h \
    utils/myapp.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target 

RESOURCES += \
    res.qrc
