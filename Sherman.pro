#-------------------------------------------------
#
# Project created by QtCreator 2015-07-06T20:14:46
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sherman
TEMPLATE = app

mac {
    ICON = MyIcon.icns
    LIBS += -framework Foundation
    LIBS += -framework AppKit
    CONFIG += objective_c
    OBJECTIVE_SOURCES += src/mac/deleter.mm
}

win32 {
    RC_FILE = win/app.rc
    SOURCES += src/win/deleter.cpp
}

linux {
    SOURCES += src/linux/deleter.cpp
}

SOURCES += src/main.cpp\
    src/shermanmainwin.cpp \
    src/piclabel.cpp

HEADERS  += src/shermanmainwin.h \
    src/piclabel.h \
    src/deleter.h

FORMS    += src/forms/shermanmainwin.ui \
    src/forms/about.ui

RESOURCES += resources/art.qrc
