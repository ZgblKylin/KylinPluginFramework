#include <QtCore/QtGlobal>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include "CoreDump.h"
#include <csignal>
#include "Dump.h"
#include "SystemInfo.h"

static QDir dir;
static std::function<void(int)> callback;

QStringList dumpSystemInfo()
{
    SystemInfo info;

    QStringList ret;

    QLocale locale = QLocale::system();

    auto CpuState = [&locale](const CPUState& cpuState)
    {
        QStringList ret;
        ret << QStringLiteral("    Used Rate: %1%").arg(cpuState.userate, 0, 'f', 2, QLatin1Char('0'));
        ret << QStringLiteral("    System Time: %1").arg(locale.toString(cpuState.system, 'f'));
        ret << QStringLiteral("    User Time: %1").arg(locale.toString(cpuState.user, 'f'));
        ret << QStringLiteral("    Nice Time: %1").arg(locale.toString(cpuState.nice, 'f'));
        ret << QStringLiteral("    Idle Time: %1").arg(locale.toString(cpuState.idle, 'f'));
        return ret;
    };
    CompleteCPUState cpuState = info.cpuState();
    ret << QStringLiteral("Cpu State");
    ret << QStringLiteral("  Total State:");
    ret << CpuState(cpuState.total);
    for (int i = 0; i < cpuState.childCore.count(); ++i)
    {
        CPUState coreState = cpuState.childCore.at(i);
        ret << QStringLiteral("  Core %1 State:").arg(i);
        ret << CpuState(coreState);
    }
    ret[ret.count() - 1] += QLatin1Char('\n');

    MemoryState memoryState = info.memoryState();
    ret << QStringLiteral("Memory State");
    ret << QStringLiteral("  Used Rate: %1%").arg(memoryState.userate, 0, 'f', 2, QLatin1Char('0'));
    ret << QStringLiteral("  Total Physics Memory: %1").arg(locale.toString(memoryState.totalPhys.convertToB().value(), 'f', 0));
    ret << QStringLiteral("  Free Pyhsics Memory: %1").arg(locale.toString(memoryState.freePhys.convertToB().value(), 'f', 0));
    ret << QStringLiteral("  Total Virtual Memory: %1").arg(locale.toString(memoryState.totalVirtual.convertToB().value(), 'f', 0));
    ret << QStringLiteral("  Total Page: %1").arg(locale.toString(memoryState.totalPage, 'f', 0));
    ret << QStringLiteral("  Free Page: %1\n").arg(locale.toString(memoryState.freePage, 'f', 0));

    ret << QStringLiteral("Disk State");
    for (const QFileInfo& drive : QDir::drives())
    {
        DiskState diskState = info.diskState(drive.absoluteFilePath());
        ret << QStringLiteral(" Drive %1").arg(drive.absoluteFilePath());
        ret << QStringLiteral("    Used Rate: %1%").arg(diskState.userate, 0, 'f', 2, QLatin1Char('0'));
        ret << QStringLiteral("    Block Size: %1").arg(locale.toString(diskState.blockSize, 'f', 0));
        ret << QStringLiteral("    Free Size: %1").arg(locale.toString(diskState.freeSize.convertToB().value(), 'f', 0));
        ret << QStringLiteral("    Available Size: %1").arg(locale.toString(diskState.availableSize.convertToB().value(), 'f', 0));
    }

    return ret;
}

[[noreturn]] void signalHandler(int sig)
{
    if (callback) {
        callback(sig);
    }

    QString dumpProcessOutput = ::dump(dir);

    QString dateTime = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh_mm_ss"));

    QByteArray signal;
    switch(sig)
    {
    case SIGINT:
        signal = "SIGINT";
        break;
    case SIGILL:
        signal = "SIGILL";
        break;
    case SIGFPE:
        signal = "SIGFPE";
        break;
    case SIGSEGV:
        signal = "SIGSEGV";
        break;
    case SIGTERM:
        signal = "SIGTERM";
        break;
    case SIGABRT:
        signal = "SIGABRT";
        break;
    default:
        signal = QByteArray::number(sig);
        break;
    };

    QFile file(dir.absoluteFilePath(dateTime + QStringLiteral(".txt")));
    file.open(QFile::WriteOnly | QFile::Truncate | QFile::Text);

    QTextStream ts(&file);
    ts.setCodec(QTextCodec::codecForName("UTF-8"));

    qDebug() << "Signal" << signal.constData();
    ts << "Signal " << signal << endl;
    qDebug() << "========";
    ts << "======== " << endl;

    qDebug();
    ts << endl;
    qDebug() << "Generating core dump ...";
    ts << "Generating core dump ..." << endl;
    qDebug() << dumpProcessOutput.toUtf8().constData();
    ts << dumpProcessOutput.toUtf8().constData();
    qDebug() << "Core dump generated";
    ts << "Core dump generated" << endl;

    qDebug();
    ts << endl;
    qDebug() << "Current Call Stack:";
    ts << "Current Call Stack:" << endl;
    for (const QString& stack : ::stackWalk())
    {
        qDebug() << stack.toUtf8().constData();
        ts << stack.toUtf8().constData() << endl;
    }

    qDebug();
    ts << endl;
    for (const QString& str : ::dumpSystemInfo())
    {
        qDebug() << str.toUtf8().constData();
        ts << str.toUtf8().constData() << endl;
    }

    file.flush();
    while(file.waitForBytesWritten(100)) {}
    file.close();

    QThread::currentThread()->terminate();
#ifdef Q_OS_WIN
    ::TerminateProcess(::GetCurrentProcess(), UINT(-sig));
#elif defined(Q_OS_LINUX)
    QProcess::execute(QStringLiteral("kill"),
                      { QStringLiteral("-9"),
                        QString::number(qApp->applicationPid()) });
#endif
    std::exit(-sig);
}

[[noreturn]] void terminateHandler()
{
    signalHandler(SIGTERM);
}

CoreDump::CoreDump(QObject* parent)
    : QObject(parent)
{
    dir = QDir(qApp->applicationDirPath()
               + QDir::separator()
               + QStringLiteral("Dump"));

    if (!dir.exists()) {
        dir.mkpath(dir.absolutePath());
    }

    std::signal(SIGINT, &signalHandler);
    std::signal(SIGILL, &signalHandler);
    std::signal(SIGFPE, &signalHandler);
    std::signal(SIGSEGV, &signalHandler);
    std::signal(SIGTERM, &signalHandler);
    std::signal(SIGABRT, &signalHandler);
    std::set_terminate(&terminateHandler);
}

CoreDump::~CoreDump()
{
}

QStringList CoreDump::stackWalk()
{
    return ::stackWalk();
}

QStringList CoreDump::dumpSystemInfo()
{
    return ::dumpSystemInfo();
}

void CoreDump::dump(const QString& dir)
{
    ::dump(QDir(dir));
}

void CoreDump::registerCallback(std::function<void(int)> cb)
{
    callback = cb;
}
