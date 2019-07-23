#include "KpfLogPrivate.h"
#include "KpfPrivate.h"

Kpf::LogDisplayPage::LogDisplayPage()
{
    auto buffer = kpfObject.findObject<log4qt::LogDisplayBuffer>(
                      QStringLiteral("LogDisplayBuffer"));
    if (buffer) {
        setBuffer(buffer);
    }
}

Kpf::LogDisplayWidget::LogDisplayWidget()
{
    auto buffer = kpfObject.findObject<log4qt::LogDisplayBuffer>(
                      QStringLiteral("LogDisplayBuffer"));
    if (buffer) {
        setBuffer(buffer);
    }
}

void Kpf::initLogger(int argc, char* argv[])
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    log4qt::LogDebugOutput* logDebugOutput;
    logDebugOutput = kpfObject.createObject<log4qt::LogDebugOutput>(
                         QStringLiteral("logDebugOutput"),
                         QStringLiteral("LogDebugOutput"));
    if (logDebugOutput) {
        logDebugOutput->start();
    }

    log4qt::LogFileMmapSaver* logFileMmapSaver;
    logFileMmapSaver = kpfObject.createObject<log4qt::LogFileMmapSaver>(
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
        logFileMmapSaver->setFilter(QtCriticalMsg);
        logFileMmapSaver->start();
    }

    log4qt::LogFileNormalSaver* logFileNormalSaver;
    logFileNormalSaver = kpfObject.createObject<log4qt::LogFileNormalSaver>(
                             QStringLiteral("logFileNormalSaver"),
                             QStringLiteral("LogFileNormalSaver"));
    if (logFileNormalSaver) {
        logFileNormalSaver->start();
    }

    qCInfo(kpf) << "Log engine initialized";
}
