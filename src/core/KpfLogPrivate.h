#pragma once
#include <Kpf/Kpf.h>
#include <Kpf/Log.h>
#include <Kpf/Log/Processor/LogDebugOutput.h>
#include <Kpf/Log/Processor/LogFileNormalSaver.h>
#include <Kpf/Log/Processor/LogFileMmapSaver.h>
#include <Kpf/Log/Processor/LogDisplayBuffer.h>
#include <Kpf/Log/Processor/LogDisplayPage.h>
#include <Kpf/Log/Processor/LogDisplayWidget.h>

KPF_REGISTER_CLASS(Log::Impl::LogDebugOutput, LogDebugOutput)
KPF_REGISTER_CLASS(Log::Impl::LogFileNormalSaver, LogFileNormalSaver)
KPF_REGISTER_CLASS(Log::Impl::LogFileMmapSaver, LogFileMmapSaver)
KPF_REGISTER_CLASS(Log::Impl::LogDisplayBuffer, LogDisplayBuffer)
KPF_REGISTER_CLASS(Log::Impl::LogDisplayPage, LogDisplayPage)
KPF_REGISTER_CLASS(Log::Impl::LogDisplayWidget, LogDisplayWidget)

namespace Kpf {
void initLogger(int argc, char* argv[]);
} // namespace Kpf
