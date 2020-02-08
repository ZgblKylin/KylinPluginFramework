CONFIG(release, debug|release): LIBS += -L$$PWD/bin/ -lKPF
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/bin/ -lKPFd

INCLUDEPATH += $$PWD/include

HEADERS += \
    $$PWD/include/Kpf/Common.h \
    $$PWD/include/Kpf/Constants.h \
    $$PWD/include/Kpf/InvokeHelper.h \
    $$PWD/include/Kpf/Connection.h \
    $$PWD/include/Kpf/Event.h \
    $$PWD/include/Kpf/Topic.h \
    $$PWD/include/Kpf/Thread.h \
    $$PWD/include/Kpf/Object.h \
    $$PWD/include/Kpf/Class.h \
    $$PWD/include/Kpf/KpfCore.h \
    $$PWD/include/Kpf/EventHelper.h \
    $$PWD/include/Kpf/Kpf.h
