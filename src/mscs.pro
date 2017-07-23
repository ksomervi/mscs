#-------------------------------------------------
#
# Project created by QtCreator 2017-02-11T07:24:19
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mscs
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    portconfigdialog.cpp

HEADERS  += mainwindow.h \
    portconfigdialog.h

FORMS += \
    portconfigdialog.ui
