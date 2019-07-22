#-------------------------------------------------
#
# Project created by QtCreator 2018-06-01T15:45:14
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib
CONFIG += c++11

DEFINES += TEST_LIBRARY

win32:CONFIG(release, debug|release): TARGET = Test
else:win32:CONFIG(debug, debug|release): TARGET = Testd
else:unix:!macx: TARGET = Test

DESTDIR = $$_PRO_FILE_PWD_/../../../bin/plugins

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
    Test.cpp

HEADERS += \
    Test.h

DISTFILES += \
    $$PWD/../../../bin/components/test.json

include($$PWD/../../../Kpf.pri)
