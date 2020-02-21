DESTDIR = $$PWD/../../bin

win32:CONFIG(debug,debug|release):TARGET = pnrd
else:win32:CONFIG(release,debug|release):TARGET = pnr

TEMPLATE = lib
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt


include(../../image_recognizing/opencv/opencv.pri)
include(../../image_recognizing/easypr/easypr.pri)
include(../../image_recognizing/tesseract-ocr/tesseract-ocr.pri)
include(../../image_processing/image_adjust/image_adjust.pri)


INCLUDEPATH += $$PWD/src

SOURCES += \
    $$PWD/src/platelocate.cpp \
    $$PWD/src/charscutting.cpp \
    $$PWD/src/charsrecognize.cpp \
    $$PWD/carnumrecognize.cpp \
    src/common.cpp \
    src/charfeature.cpp \
    src/cxann.cpp \
    src/cxsvm.cpp

HEADERS += \
    $$PWD/src/charscutting.h \
    $$PWD/src/charsrecognize.h \
    $$PWD/src/common.h \
    $$PWD/src/platelocate.h \
    $$PWD/carnumrecognize.h \
    src/charfeature.h \
    src/cxann.h \
    src/cxsvm.h

