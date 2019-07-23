#pragma once
#include <Kpf/Kpf.h>
#include <processors/LogDebugOutput.h>
#include <processors/LogFileSaver.h>
#include <processors/LogDisplayWidget.h>

namespace Kpf {
class LogDisplayPage : public log4qt::LogDisplayPage
{
    Q_OBJECT
public:
    LogDisplayPage();
};

class LogDisplayWidget : public log4qt::LogDisplayWidget
{
    Q_OBJECT
public:
    LogDisplayWidget();
};

void initLogger(int argc, char* argv[]);
} // namespace Kpf

KPF_REGISTER_CLASS(log4qt::LogDebugOutput, LogDebugOutput)
KPF_REGISTER_CLASS(log4qt::LogFileNormalSaver, LogFileNormalSaver)
KPF_REGISTER_CLASS(log4qt::LogFileMmapSaver, LogFileMmapSaver)
KPF_REGISTER_CLASS(log4qt::LogDisplayBuffer, LogDisplayBuffer)
KPF_REGISTER_CLASS(Kpf::LogDisplayPage, LogDisplayPage)
KPF_REGISTER_CLASS(Kpf::LogDisplayWidget, LogDisplayWidget)
