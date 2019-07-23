#pragma once
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include "DataSizeHelper.h"

// 自动推送标记
class AutoPush
{
    Q_GADGET
public:
    enum T
    {
        None         = 0x0000,
        CPUState     = 0x0001,
        MemoryState  = 0x0002,
        DiskState    = 0x0004,
        All          = None | CPUState | MemoryState | DiskState,
    };
    Q_DECLARE_FLAGS(Flag, T)
    Q_FLAGS(Flag)
};
Q_DECLARE_METATYPE(AutoPush::Flag)

//< CPU，子核心ID从0开始
enum class CPUCoreId
{
    ID_total = -1, //< 总的核心ID
};

// CPU状态
struct CPUState
{
    double userate = 0.0;    //< CPU 使用率，范围[0~100]
    double system = 0.0;     //< 内核使用时间
    double user = 0.0;       //< 用户使用时间
    double nice = 0.0;       //< 优先使用时间
    double idle = 0.0;       //< 空闲时间

    CPUState& operator+=(const CPUState& other)
    {
        system += other.system;
        user += other.user;
        idle += other.idle;
        nice += other.nice;
        userate = 0.0; //< 使用率更新为0
        return *this;
    }

    friend CPUState operator-(const CPUState& arg1, const CPUState& arg2)
    {
        CPUState value;
        value.system = arg1.system - arg2.system;
        value.user = arg1.user - arg2.user;
        value.idle = arg1.idle - arg2.idle;
        value.nice = arg1.nice - arg2.nice;
        value.userate = 0.0; //< 使用率更新为0
        return value;
    }
};
Q_DECLARE_METATYPE(CPUState)
using CPUStateList = QVector<CPUState>;

struct CompleteCPUState
{
    CPUState total; //< 总共的CPU状态
    CPUStateList childCore; //< 子核心的CPU状态
};
Q_DECLARE_METATYPE(CompleteCPUState)

// 内存状态
struct MemoryState
{
    double userate = 0.0;    //< 内存使用率，范围[0~100]
    DataSize totalPhys;      // 总物理内存
    DataSize freePhys;       // 空闲物理内存
    DataSize totalVirtual;   // 总虚拟内存
    DataSize freeVirtual;    // 空闲虚拟内存
    double totalPage = 0.0;  // 总页面
    double freePage = 0.0;   // 空闲页面
};
Q_DECLARE_METATYPE(MemoryState)

// 磁盘状态
struct DiskState
{
    double userate = 0.0;     //< 磁盘使用率，范围[0~100]
    double blockSize;       //< 块大小
    DataSize totalSize;       //< 总空间
    DataSize freeSize;        //< 空闲空间
    DataSize availableSize;   //< 可用空间
};
Q_DECLARE_METATYPE(DiskState)
typedef QVector<DiskState> DiskStateList;
