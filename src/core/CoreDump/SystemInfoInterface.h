#pragma once
#include "SystemInfoType.h"

class SystemInfoInterface
{
public:
    CompleteCPUState cpuState() const;
    MemoryState memoryState() const;
    DiskState diskState(const QString& dir) const;
    DiskStateList allDiskState() const;

private:
    static void calcCPUUseRate(CPUState& st);
    static void calcMemoryUseRate(MemoryState& st);
    static void calcDiskUseRate(DiskState& st);
};
