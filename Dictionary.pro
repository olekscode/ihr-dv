#-------------------------------------------------
#
# Project created by QtCreator 2014-11-29T16:53:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Dictionary
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    newword.cpp \
    dictionary.cpp \
    unknownwords.cpp

HEADERS  += mainwindow.h \
    newword.h \
    dictionary.h \
    unknownwords.h

FORMS    += mainwindow.ui \
    newword.ui

CONFIG += c++11

RESOURCES += \
    resources.qrc
