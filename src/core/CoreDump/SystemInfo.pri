HEADERS += \
    $$PWD/../../../../../include/generic/SystemInfo/SystemInfo_global.h \
    $$PWD/../../../../../include/generic/SystemInfo/SystemInfoType.h \
    $$PWD/../../../../../include/generic/SystemInfo/SystemInfo.h \
    $$PWD/SystemInfoInterface.h \
    $$PWD/SystemInfoPrivate.h

SOURCES += \
    $$PWD/SystemInfo4Windows.cpp \
    $$PWD/SystemInfo4Unix.cpp \
    $$PWD/SystemInfo.cpp
