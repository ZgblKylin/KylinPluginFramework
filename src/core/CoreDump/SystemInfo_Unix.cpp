#include "SystemInfoInterface.h"

#ifdef Q_OS_UNIX

#include <sys/statfs.h>
#include <QFile>
#include <QStringList>
#include <QTextStream>

enum MemInfoFlag
{
    None         = 0x0000,
    MemTotal     = 0x0001,
    MemFree      = 0x0002,
    SwapTotal    = 0x0004,
    SwapFree     = 0x0008,
    All          = MemTotal | MemFree | SwapTotal | SwapFree,
};

CompleteCPUState SystemInfoInterface::cpuState() const
{
    static CPUStateList lastCpuStatuList;
    CPUStateList cpuDiffStatuList;
    CPUStateList cpuTempStatuList;
    // linux 下通过读取系统保存的状态文件中的数据来获取CPU使用率
    QFile file(QStringLiteral("/proc/stat"));
    if (!file.open(QFile::ReadOnly))
    {
        return CompleteCPUState();
    }
    for (int coreIndex = 0; ; ++coreIndex)
    {
        QByteArray dataArray = file.readLine();
        if (!dataArray.startsWith(QByteArrayLiteral("cpu")))
        {
            break;
        }
        qlonglong user = 0L;
        qlonglong nice = 0L;
        qlonglong system = 0L;
        qlonglong idle = 0L;
        char name[20];
        QTextStream stream(&dataArray);
        stream >>name >>user >>system >>idle;
        CPUState cpuState;
        cpuState.user = user;
        cpuState.nice = nice;
        cpuState.system = system;
        cpuState.idle = idle;
        cpuTempStatuList.push_back(cpuState);
        CPUState diffCpuState = cpuState - lastCpuStatuList.value(coreIndex);
        cpuDiffStatuList.push_back(diffCpuState);
    }
    file.close();
    lastCpuStatuList = cpuTempStatuList;
    for (CPUState& state : cpuDiffStatuList)
    {
        calcCPUUseRate(state);
    }
    CompleteCPUState stateReturn;
    stateReturn.total = cpuDiffStatuList.last();
    cpuDiffStatuList.removeFirst();
    stateReturn.childCore = cpuDiffStatuList;
    return stateReturn;
}

MemoryState SystemInfoInterface::memoryState() const
{
    // linux 下通过读取系统保存的状态文件中的数据来获取内存使用信息
    MemoryState memoryStatus;
    QFile file(QStringLiteral("/proc/meminfo"));
    if (!file.open(QFile::ReadOnly))
    {
        return memoryStatus;
    }
    int flag = None;
    char name[30];
    char type[30];
    qlonglong value;
    while (flag != All)
    {
        QByteArray dataArray = file.readLine();
        if (dataArray.isEmpty())
        {
            break;
        }
        QTextStream stream(&dataArray);
        stream >>name >>value >>type;
        value <<= 10;
        if (strcmp(name, "MemTotal:") == 0) {
            memoryStatus.totalPhys = DataSize(value, DataSizeUnit::B);
            flag |= MemTotal;
        } else if (strcmp(name, "MemFree:") == 0) {
            memoryStatus.freePhys = DataSize(value, DataSizeUnit::B);
            flag |= MemFree;
        } else if (strcmp(name, "SwapTotal:") == 0) {
            memoryStatus.totalPage = value;
            flag |= SwapTotal;
        } else if (strcmp(name, "SwapFree:") == 0) {
            memoryStatus.freePage = value;
            flag |= SwapFree;
        }
    }
    file.close();
    // 计算使用率
    calcMemoryUseRate(memoryStatus);
    return memoryStatus;
}

DiskState SystemInfoInterface::diskState(const QString& dir) const
{
    /**
     * @brief windows 下通过调用 statfs 来获取磁盘的使用情况
     */
    struct statfs diskInfo;

    statfs(dir.toStdString().data(), &diskInfo);

    DiskState diskStatus;
    diskStatus.blockSize = diskInfo.f_bsize;
    diskStatus.totalSize = DataSize(diskInfo.f_blocks * diskStatus.blockSize, DataSizeUnit::B);

    diskStatus.freeSize = DataSize(diskInfo.f_bfree * diskStatus.blockSize, DataSizeUnit::B);
    diskStatus.availableSize = DataSize(diskInfo.f_bavail * diskStatus.blockSize, DataSizeUnit::B);
    // 计算使用率
    calcDiskUseRate(diskStatus);
    return diskStatus;
}

DiskStateList SystemInfoInterface::allDiskState() const
{
    DiskStateList stList;
    // linux 下通过读取系统保存的状态文件获取所有的磁盘分区
    QFile file(QStringLiteral("/proc/mounts"));
    if (!file.open(QFile::ReadOnly))
    {
        return stList;
    }
    QByteArray dataArray;

    char dev[30];
    char mount[30];
    char type[30];

    while (!file.atEnd())
    {
        dataArray = file.readLine();
        QTextStream stream(&dataArray);
        stream >>dev >>mount >>type;
        if (strcmp(type, "devtmpfs") == 0
            || strcmp(type, "tmpfs") == 0
            || strcmp(type, "ext2") == 0
            || strcmp(type, "ext3") == 0
            || strcmp(type, "ext4") == 0)
        {
            DiskState diskst = diskState(mount);
            stList.push_back(diskst);
        }
    }
    file.close();
    return stList;
}

void SystemInfoInterface::calcCPUUseRate(CPUState& st)
{
    double usage = st.system + st.user;
    double total = st.system + st.user + st.idle + st.nice;
    if (qFuzzyIsNull(total))
    {
        st.userate = 0;
    }
    st.userate = 100.0 * usage / total;
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

#endif // Q_OS_UNIX
