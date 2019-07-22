#include "KpfLogPrivate.h"
#include "KpfPrivate.h"

using namespace Log::Impl;
using namespace Kpf;

ILogEngine* Kpf::getLogEngine()
{
    return &(LogEngine::instance());
}

void Kpf::initLogger(int argc, char* argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    ILogEngine* engine = getLogEngine();

    engine->setCategoryFilter(QStringLiteral("Kpf"), 1);

    LogDebugOutput* logDebugOutput;
    logDebugOutput = kpfObject.createObject<LogDebugOutput>(
                         QStringLiteral("logDebugOutput"),
                         QStringLiteral("LogDebugOutput"));
    Q_UNUSED(logDebugOutput);

    LogFileMmapSaver* logFileMmapSaver;
    logFileMmapSaver = kpfObject.createObject<LogFileMmapSaver>(
                           QStringLiteral("logFileMmapSaver"),
                           QStringLiteral("LogFileMmapSaver"));
    if (logFileMmapSaver)
    {
        QDir dir(qApp->applicationDirPath()
                 + QDir::separator()
                 + QStringLiteral("Log_Critical"));
        if (!dir.exists()) {
            dir.mkpath(dir.absolutePath());
        }
        logFileMmapSaver->setDir(dir.absolutePath());
        logFileMmapSaver->setFilter(Log::Critical);
    }

    LogFileNormalSaver* logFileNormalSaver;
    logFileNormalSaver = kpfObject.createObject<LogFileNormalSaver>(
                             QStringLiteral("logFileNormalSaver"),
                             QStringLiteral("LogFileNormalSaver"));
    Q_UNUSED(logFileNormalSaver);

    kpfCInformation("Kpf") << "Log engine initialized";
}
