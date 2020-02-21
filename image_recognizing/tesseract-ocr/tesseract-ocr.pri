
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/tesseract-ocr/lib/ -ltesseract
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/tesseract-ocr/lib/ -ltesseractd
else:unix: LIBS += -L$$PWD/tesseract-ocr/lib/ -ltesseract

INCLUDEPATH += \
    $$PWD/tesseract-ocr/src/include/ \



