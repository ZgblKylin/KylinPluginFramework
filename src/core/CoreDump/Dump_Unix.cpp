#include "Dump.h"
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>

QString dump(const QDir& dir)
{
    QProcess process;
    process.setProgram(QStringLiteral("procdump"));
    process.setArguments({
                             QStringLiteral("-p"),
                             QString::number(qApp->applicationPid())
                         });
    process.setWorkingDirectory(dir.absolutePath());
    process.setProcessChannelMode(QProcess::ForwardedChannels);
    process.start();
    process.waitForFinished(-1);
    return QString::fromUtf8(process.readAll());
}

QStringList stackWalk()
{
    QStringList ret;

    void* callStack[256];
    size_t size;
    char** strings;

    size = backtrace(callStack, 256);
    strings = backtrace_symbols(callStack, size);

    for(size_t i=0;i<size;++i)
        ret << strings[i];

    return ret;
}
