#-------------------------------------------------
#
# Project created by QtCreator 2018-07-03T22:30:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TinyNotepad
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    dialogabout.cpp \
    dialoghelp.cpp \
    dialogfind.cpp \
    chinesedisplaytool.cpp \
    codeeditor.cpp \
    highlighter.cpp

HEADERS += \
        mainwindow.h \
    dialogabout.h \
    dialoghelp.h \
    dialogfind.h \
    chinesedisplaytool.h \
    codeeditor.h \
    highlighter.h

FORMS += \
        mainwindow.ui \
    dialogabout.ui \
    dialoghelp.ui \
    dialogfind.ui

DISTFILES +=

RESOURCES += \
    res.qrc

#打印功能
QT += printsupport

#程序图标文件,放置到与源代码文件相同目录下
RC_ICONS = TinyNotepad.ico
