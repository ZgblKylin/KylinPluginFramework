#-------------------------------------------------
#
# Project created by QtCreator 2018-06-01T20:31:10
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += c++11

win32:CONFIG(release, debug|release): TARGET = app
else:win32:CONFIG(debug, debug|release): TARGET = appd
else:unix:!macx: TARGET = app
DESTDIR = $$PWD/../../bin

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
    MainWindow.cpp

HEADERS += \
    MainWindow.h

FORMS += \
    MainWindow.ui

DISTFILES += \
    $$PWD/../../bin/config/app.json \
    $$PWD/../../bin/components/test.json \
    $$PWD/../../bin/config/app.xml \
    $$PWD/../../bin/components/test.xml

include($$PWD/../../Kpf.pri)
