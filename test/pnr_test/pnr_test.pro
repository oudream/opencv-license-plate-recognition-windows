#-------------------------------------------------
#
# Project created by QtCreator 2016-06-15T10:15:57
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR = $$PWD/../../bin

TARGET = pnr_test
TEMPLATE = app

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../bin -lpnr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../bin -lpnrd

INCLUDEPATH += $$PWD/../../app/car_number_recognition/src
INCLUDEPATH += $$PWD/pnr/
DEPENDPATH += $$PWD/pnr/

SOURCES += main.cpp\
        testwindow.cpp \
    pnrthread.cpp \
    trainthread.cpp

HEADERS  += testwindow.h \
    pnrthread.h \
    trainthread.h

FORMS    += testwindow.ui

