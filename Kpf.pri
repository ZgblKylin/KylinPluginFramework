CONFIG(release, debug|release): LIBS += -L$$PWD/bin/ -lKPF
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/bin/ -lKPFd

INCLUDEPATH += $$PWD/include
