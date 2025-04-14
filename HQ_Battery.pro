QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH +=\
    component \
    utils


SOURCES += \
    component/layoutbatterywidget.cpp \
    component/mytreewidget.cpp \
    main.cpp \
    mainwindow.cpp \
    protocols/productprocessors.cpp \
    protocols/protocol.cpp \
    protocols/serialdataprocessor.cpp \
    protocols/serialworker.cpp

HEADERS += \
    component/layoutbatterywidget.h \
    component/mytreewidget.h \
    mainwindow.h \
    protocols/productprocessors.h \
    protocols/protocol.h \
    protocols/serialdataprocessor.h \
    protocols/serialworker.h \
    utils/Struct.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target 

RESOURCES += \
    res.qrc
