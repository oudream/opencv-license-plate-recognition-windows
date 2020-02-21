CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

CONFIG(debug, debug|release) : DEFINES += DEBUG
CONFIG(release, debug|release) : DEFINES += NDEBUG

#include(../../image_recognizing/opencv/opencv.pri)

INCLUDEPATH += \
#    $$PWD/include/3rd_party \
    $$PWD/src

SOURCES += \
    $$PWD/src/imageadjust.cpp

HEADERS += \
    $$PWD/src/imageadjust.h
