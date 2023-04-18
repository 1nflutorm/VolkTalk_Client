QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

VERSION = 1.1.3

QMAKE_TARGET_COMPANY = 1nflutorm Co
QMAKE_TARGET_PRODUCT = VolkTalk
QMAKE_TARGET_DESCRIPTION = A simple messenger on TcpSockets
QMAKE_TARGET_COPYRIGHT = 1nflutorm

win32:RC_FILE = icon.rc

SOURCES += \
    creategroupform.cpp \
    groupinfoform.cpp \
    main.cpp \
    clientwindow.cpp \
    messagebox.cpp \
    userinfoform.cpp

HEADERS += \
    clientwindow.h \
    creategroupform.h \
    groupinfoform.h \
    messagebox.h \
    userinfoform.h

FORMS += \
    clientwindow.ui \
    creategroupform.ui \
    groupinfoform.ui \
    messagebox.ui \
    userinfoform.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resourses.qrc
