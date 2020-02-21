TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

TARGET = ann_train

include(../../image_recognizing/opencv/opencv.pri)
include(../../image_recognizing/easypr/easypr.pri)

INCLUDEPATH += $$PWD/include/
DEPENDPATH += $$PWD/include/

SOURCES += main.cpp



