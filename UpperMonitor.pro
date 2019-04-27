#-------------------------------------------------
#
# Project created by QtCreator 2019-03-14T14:43:21
#
#-------------------------------------------------

QT       += core gui
QT       += serialport

RC_ICONS = icon.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UpperMonitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES +=

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main/main.cpp \
        main/uppermain.cpp \
        custom/dialog.cpp \
        custom/help.cpp \
        component/DoubleClickButton.cpp \
        component/ClickLabel.cpp \
    custom/QuitMsg.cpp


HEADERS += \
        main/uppermain.h \
        custom/dialog.h \
        custom/help.h \
        component/DoubleClickButton.h \
        component/ClickLabel.h \
    custom/QuitMsg.h



FORMS += \
        main/uppermain.ui \
        custom/dialog.ui \
        custom/help.ui \
    custom/quitmsg.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config.ini \

RESOURCES += \
    resource/image.qrc
