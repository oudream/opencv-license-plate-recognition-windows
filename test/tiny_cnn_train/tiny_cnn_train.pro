TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += $$PWD/tiny_cnn/
#DEPENDPATH += $$PWD/tiny_cnn/
include(../../image_recognizing/opencv/opencv.pri)

SOURCES += \
    examples/mnist/train.cpp

HEADERS += \



