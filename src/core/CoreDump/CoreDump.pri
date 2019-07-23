HEADERS += \
    $$PWD/SystemInfoInterface.h \
    $$PWD/SystemInfoPrivate.h \
    $$PWD/SystemInfoType.h \
    $$PWD/CoreDump.h \
    $$PWD/DataSizeHelper.h \
    $$PWD/Dump.h \
    $$PWD/SystemInfo.h

SOURCES += \
    $$PWD/CoreDump.cpp \
    $$PWD/SystemInfo.cpp \

win32 {
SOURCES += \
    $$PWD/SystemInfo_Windows.cpp \
    $$PWD/Dump_Windows.cpp

    LIBS += -ldbghelp
    LIBS += -lntdll
}

unix {
SOURCES += \
    $$PWD/SystemInfo_Unix.cpp \
    $$PWD/Dump_Unix.cpp
}
