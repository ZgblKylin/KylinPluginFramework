TEMPLATE = lib
DESTDIR = $$PWD/../../../../../bin
CONFIG(release, debug | release) : TARGET = Debugging
CONFIG(debug, debug | release) : TARGET = Debuggingd

DEFINES += DEBUGGING_LIBRARY

include(Debugging.pri)
