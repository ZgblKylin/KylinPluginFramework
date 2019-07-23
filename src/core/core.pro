#-------------------------------------------------
#
# Project created by QtCreator 2018-06-01T12:38:36
#
#-------------------------------------------------

QT       += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG(release, debug|release): TARGET = KPF
else:CONFIG(debug, debug|release): TARGET = KPFd

TEMPLATE = lib
CONFIG += c++11

DESTDIR = $$PWD/../../bin

DEFINES += KPF_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $$PWD/../../include

HEADERS += \
    $$PWD/../../include/Kpf/Common.h \
    $$PWD/../../include/Kpf/Constants.h \
    $$PWD/../../include/Kpf/InvokeHelper.h \
    $$PWD/../../include/Kpf/Connection.h \
    $$PWD/../../include/Kpf/Event.h \
    $$PWD/../../include/Kpf/Topic.h \
    $$PWD/../../include/Kpf/Thread.h \
    $$PWD/../../include/Kpf/Object.h \
    $$PWD/../../include/Kpf/Class.h \
    $$PWD/../../include/Kpf/KpfCore.h \
    $$PWD/../../include/Kpf/EventHelper.h \
    $$PWD/../../include/Kpf/Kpf.h \
    SignalSpy.h \
    ClassImpl.h \
    ThreadImpl.h \
    ObjectImpl.h \
    EventImpl.h \
    TopicImpl.h \
    EventBus.h \
    KpfCoreImpl.h \
    KpfLogPrivate.h \
    KWidgets.h \
    KpfPrivate.h \
    InvokeHelperPrivate.h \
    CommonPrivate.h \
    ConnectionImpl.h \
    Library.h \
    RegisterQtClasses.h

SOURCES += \
    CommonPrivate.cpp \
    InvokeHelper.cpp \
    SignalSpy.cpp \
    Class.cpp \
    Thread.cpp \
    Object.cpp \
    Event.cpp \
    Topic.cpp \
    EventBus.cpp \
    Connection.cpp \
    KpfCore.cpp \
    KpfLog.cpp \
    KWidgets.cpp \
    Library.cpp

DISTFILES += \
    $$PWD/../../bin/components/KWidgets.json

include(CoreDump/CoreDump.pri)

DEFINES += LOG4QT_STATIC
include(../../log4qt/src/log4qt.pri)
