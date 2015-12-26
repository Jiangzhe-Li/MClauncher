#-------------------------------------------------
#
# Project created by QtCreator 2015-12-23T16:20:08
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MClauncher
TEMPLATE = app


SOURCES += \
    settingdialog.cpp \
    MClauncherCore.cpp \
    mclauncher.cpp \
    main.cpp \
    infodialog.cpp

HEADERS  += \
    settingdialog.h \
    MClauncherCore.h \
    mclauncher.h \
    infodialog.h

FORMS    += \
    settingdialog.ui \
    mclauncher.ui \
    infodialog.ui

RESOURCES += \
    res.qrc
