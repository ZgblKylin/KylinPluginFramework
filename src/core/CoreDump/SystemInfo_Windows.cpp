#include "SystemInfoInterface.h"

#ifdef Q_OS_WIN

#include <windows.h>
#include <Winternl.h>

using CPUInfo4Win = SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;
using CPUInfo4WinList = QVector<CPUInfo4Win>;
using SystemInfo4Win = SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

CompleteCPUState SystemInfoInterface::cpuState() const
{
    static CompleteCPUState lastCompleteCPUState; //< 最后一次的CPU状态
    static int coreNumber = 0; //< CPU核心数不可能为 0，如果为 0 则表示未初始化
    if (coreNumber == 0)
    {
        // 获取核心数，并填充子核心状态Vector
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        coreNumber = static_cast<int>(sys_info.dwNumberOfProcessors);
        lastCompleteCPUState.childCore.resize(coreNumber);
    }
    // 获取信息
    SystemInfo4Win systemInfo[1024];
    NtQuerySystemInformation(SystemProcessorPerformanceInformation,
                             &systemInfo,
                             sizeof(SystemInfo4Win) * static_cast<ULONG>(coreNumber),
                             nullptr);
    CompleteCPUState currentCompleteCPUState; //< 当前完整的CPU状态
    CompleteCPUState diffCompleteCPUState; //< 相差的CPU状态
    // 遍历每个核的信息，并计算总的信息
    for (int corIndex = 0; corIndex != coreNumber; corIndex++)
    {
        SystemInfo4Win& cpuInfo = systemInfo[corIndex];

        // 构建当前单个核心的状态
        CPUState singleCoreState;
        singleCoreState.system = cpuInfo.KernelTime.QuadPart;
        singleCoreState.idle = cpuInfo.IdleTime.QuadPart;
        singleCoreState.user = cpuInfo.UserTime.QuadPart;

        // 添加到子核心状态列表中，并更新总的CPU信息
        currentCompleteCPUState.total += singleCoreState;
        currentCompleteCPUState.childCore.push_back(singleCoreState);

        // 计算与最后一次核心数据的偏差，并保存
        CPUState diffToLastState = singleCoreState
                - lastCompleteCPUState.childCore.value(corIndex);
        // 计算使用率
        calcCPUUseRate(diffToLastState);
        // 保存
        diffCompleteCPUState.childCore.push_back(diffToLastState);
        diffCompleteCPUState.total += diffToLastState;
    }
    // 计算使用率
    calcCPUUseRate(diffCompleteCPUState.total);
    // 更新最后一次的CPU状态信息
    lastCompleteCPUState = currentCompleteCPUState;
    return diffCompleteCPUState;
}

MemoryState SystemInfoInterface::memoryState() const
{
    MEMORYSTATUSEX memory;
    memory.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memory);

    // 构建内存状态
    MemoryState state;
    state.totalPhys = DataSize(memory.ullTotalPhys, DataSizeUnit::B);
    state.freePhys = DataSize(memory.ullAvailPhys, DataSizeUnit::B);
    state.totalVirtual = DataSize(memory.ullTotalVirtual, DataSizeUnit::B);
    state.freeVirtual = DataSize(memory.ullAvailVirtual, DataSizeUnit::B);
    state.totalPage = memory.ullTotalPageFile;
    state.freePage = memory.ullAvailPageFile;
    // 计算使用率
    calcMemoryUseRate(state);
    return state;
}

DiskState SystemInfoInterface::diskState(const QString& dir) const
{
    _ULARGE_INTEGER availableSize;
    _ULARGE_INTEGER totalSize;
    _ULARGE_INTEGER freeSize;
    GetDiskFreeSpaceExW(dir.toStdWString().data(),
                        &availableSize,
                        &totalSize,
                        &freeSize);
    // 构建磁盘状态
    DiskState state;
    state.blockSize = 0.0;
    state.availableSize = DataSize(static_cast<double>(availableSize.QuadPart),
                                   DataSizeUnit::B);
    state.totalSize = DataSize(static_cast<double>(totalSize.QuadPart),
                                   DataSizeUnit::B);
    state.freeSize = DataSize(static_cast<double>(freeSize.QuadPart),
                               DataSizeUnit::B);
    // 计算使用率
    calcDiskUseRate(state);
    return state;
}

DiskStateList SystemInfoInterface::allDiskState() const
{
    DiskStateList stateList;
    char drives[MAX_PATH];
    DWORD dwLen = GetLogicalDriveStringsA(MAX_PATH, drives);
    for (DWORD pos = 0; pos < dwLen;) {
        if (drives[pos] != 0) {
            const char *drive = drives + pos;
            DiskState state = diskState(QString(drive));
            stateList.push_back(state);
            pos += strlen(drive) + 1;
        }
    }
    return stateList;
}

void SystemInfoInterface::calcCPUUseRate(CPUState& st)
{
    double usage = st.system + st.user - st.idle;
    double total = st.system + st.user;
    if (qFuzzyIsNull(total)) {
        st.userate = 0;
    } else {
        st.userate = 100.0 * usage / total;
    }
}

void SystemInfoInterface::calcMemoryUseRate(MemoryState& st)
{
    double total = st.totalPhys.value();
    double usage = total - st.freePhys.value();
    if (qFuzzyIsNull(total))
    {
        st.userate = 0;
    }
    st.userate = 100.0 * usage / total;
}

void SystemInfoInterface::calcDiskUseRate(DiskState& st)
{
    double total = st.totalSize.value();
    double usage = total - st.availableSize.value();
    if (qFuzzyIsNull(total))
    {
        st.userate = 0;
    }
    st.userate = 100.0 * usage / total;
}

#endif
