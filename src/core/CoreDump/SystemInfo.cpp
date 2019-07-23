#include "SystemInfo.h"
#include "SystemInfoPrivate.h"
#include "SystemInfoInterface.h"
#include <QtCore/QtCore>
#include <QCoreApplication>

/************************************************************************
 * SystemInfoPrivate 的接口实现
 ************************************************************************/
SystemInfoPrivate::SystemInfoPrivate(SystemInfo* pthis)
    : q_ptr(pthis)
    , _interface(new SystemInfoInterface)
{
}

SystemInfoPrivate::~SystemInfoPrivate()
{
}

void SystemInfoPrivate::pushSystemInfo()
{
    Q_Q(SystemInfo);
    // 推送CPU状态
    if (_autoPush & AutoPush::CPUState)
    {
        CompleteCPUState completeCPUState = _interface->cpuState();
        // 推送总的CPU使用率
        emit q->pushCPUUsageRate(static_cast<int>(CPUCoreId::ID_total),
                                 completeCPUState.total.userate);
        // 推送各个子核心的CPU使用率
        CPUStateList& childCoreCPUState = completeCPUState.childCore;
        for (int idx = 0; idx != childCoreCPUState.count(); ++idx)
        {
            emit q->pushCPUUsageRate(idx,
                                     childCoreCPUState.value(idx).userate);
        }
        //
        // 推送检测的阈值
        pushCPURateThreshol(completeCPUState.total.userate);
    }
    // 推送物理内存状态
    if (_autoPush & AutoPush::MemoryState)
    {
        MemoryState state = _interface->memoryState();
        double remain = state.freePhys.convertToGB().value();
        double usage = (state.totalPhys - state.freePhys).convertToGB().value();
        // 推送使用量、使用率、剩余量、剩余率
        emit q->pushMemoryUsage(usage);
        emit q->pushMemoryUsageRate(state.userate);
        emit q->pushMemoryRemain(remain);
        emit q->pushMemoryRemainRate(100.0 - state.userate);
        // 推送检测的阈值
        pushMemoryRateThreshol(state.userate);
        pushMemoryUsageThreshol(usage);
    }
    // 推送磁盘状态
    if (_autoPush & AutoPush::DiskState)
    {
        for (QString dir : _pushDiskDirList)
        {
            DiskState state = _interface->diskState(dir);

            double remain = state.availableSize.convertToGB().value();
            double usage = (state.totalSize - state.availableSize).convertToGB().value();
            // 推送使用量、使用率、剩余量、剩余率
            emit q->pushDiskUsage(dir, usage);
            emit q->pushDiskUsageRate(dir, state.userate);
            emit q->pushDiskRemain(dir, remain);
            emit q->pushDiskRemainRate(dir, 100.0 - state.userate);
            // 推送检测的阈值
            pushDiskRateThreshol(dir, state.userate);
            pushDiskUsageThreshol(dir, usage);
        }
    }
}

void SystemInfoPrivate::pushCPURateThreshol(double rate)
{
    Q_Q(SystemInfo);
    // 检测需要推送的CPU使用率阈值
    const QString key = ThresholdType::CPURateIns().str();
    QVector<double> threshols;
    bool isAlter = checkPushThreshol(key,
                                     rate,
                                     threshols);
    // 推送CPU使用率阈值
    if (isAlter)
    {
        for (double value : threshols)
        {
            emit q->pushCPURateAlter(value);
        }
    }
    else
    {
        for (double value : threshols)
        {
            emit q->pushCPURateAlterClear(value);
        }
    }
}

void SystemInfoPrivate::pushMemoryRateThreshol(double rate)
{
    Q_Q(SystemInfo);
    // 检测需要推送内存使用率的阈值
    const QString key = ThresholdType::MemoryRateIns().str();
    QVector<double> threshols;
    bool isAlter = checkPushThreshol(key,
                                     rate,
                                     threshols);
    // 推送内存使用率阈值
    if (isAlter)
    {
        for (double value : threshols)
        {
            emit q->pushMemoryRateAlter(value);
        }
    }
    else
    {
        for (double value : threshols)
        {
            emit q->pushMemoryRateAlterClear(value);
        }
    }
}

void SystemInfoPrivate::pushMemoryUsageThreshol(double usage)
{
    Q_Q(SystemInfo);
    // 检测需要推送内存使用量的阈值
    const QString key = ThresholdType::MemoryUsageIns().str();
    QVector<double> threshols;
    bool isAlter = checkPushThreshol(key,
                                     usage,
                                     threshols);
    // 推送内存使用率阈值
    if (isAlter)
    {
        for (double value : threshols)
        {
            emit q->pushMemoryUsageAlter(value);
        }
    }
    else
    {
        for (double value : threshols)
        {
            emit q->pushMemoryUsageAlterClear(value);
        }
    }
}

void SystemInfoPrivate::pushDiskRateThreshol(const QString& dir, double rate)
{
    Q_Q(SystemInfo);
    // 检测需要推送磁盘使用率的阈值
    const QString key = ThresholdType::DiskRateIns().str(dir);
    QVector<double> threshols;
    bool isAlter = checkPushThreshol(key,
                                     rate,
                                     threshols);
    // 推送内存使用率阈值
    if (isAlter)
    {
        for (double value : threshols)
        {
            emit q->pushDiskRateAlter(dir, value);
        }
    }
    else
    {
        for (double value : threshols)
        {
            emit q->pushDiskRateAlterClear(dir, value);
        }
    }
}

void SystemInfoPrivate::pushDiskUsageThreshol(const QString& dir, double usage)
{
    Q_Q(SystemInfo);
    // 检测需要推送磁盘使用量的阈值
    const QString key = ThresholdType::DiskUsageIns().str(dir);
    QVector<double> threshols;
    bool isAlter = checkPushThreshol(key,
                                     usage,
                                     threshols);
    // 推送内存使用率阈值
    if (isAlter)
    {
        for (double value : threshols)
        {
            emit q->pushDiskUsageAlter(dir, value);
        }
    }
    else
    {
        for (double value : threshols)
        {
            emit q->pushDiskUsageAlterClear(dir, value);
        }
    }
}

bool SystemInfoPrivate::checkPushThreshol(const QString& key,
                                          double newValue,
                                          QVector<double>& threshols)
{
    double oldValue = _lastValue.value(key, 0.0);
    // 如果两个值相等则直接返回，需要推送的内容为空
    if (qFuzzyCompare(newValue, oldValue)) return false;
    // 获取推送阈值的类型（警告，还是清除警告）
    bool isAlter = (newValue > oldValue);
    // 警告时，推送大于旧值且小于等于新值的项
    if (isAlter)
    {
        for (double AlterValue : _alterThresholdMap.value(key))
        {
            if (AlterValue < oldValue)
            {
                continue;
            }
            else if (AlterValue <= newValue)
            {
                threshols.push_back(AlterValue);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for (double AlterValue : _alterThresholdMap.value(key))
        {
            if (AlterValue <= newValue)
            {
                continue;
            }
            else if (AlterValue <= oldValue)
            {
                threshols.push_back(AlterValue);
            }
            else
            {
                break;
            }
        }
    }
    // 更新最后的值
    _lastValue[key] = newValue;
    return isAlter;
}

/************************************************************************
 * SystemInfo 的接口实现
 ************************************************************************/
SystemInfo::SystemInfo(QObject* parent)
    : QObject(parent)
    , d_ptr(new SystemInfoPrivate(this))
{
    Q_D(SystemInfo);
    // 启动定时器
    d->_timer = new QTimer(this);
    connect(d->_timer, &QTimer::timeout, this, [this, d]()
    {
        d->pushSystemInfo();
    } );
    // 如果满足条件，则启动定时器
    if (d->_timeInterval > 0)
    {
        d->_timer->start(d->_timeInterval);
    }
}

SystemInfo::~SystemInfo()
{
}

AutoPush::Flag SystemInfo::autoPushFlag() const
{
    Q_D(const SystemInfo);
    return d->_autoPush;
}

int SystemInfo::timeInterval() const
{
    Q_D(const SystemInfo);
    return d->_timeInterval;
}

void SystemInfo::setAutoPushFlag(AutoPush::Flag value)
{
    Q_D(SystemInfo);
    d->_autoPush = value;
}

void SystemInfo::setTimeInterval(int value)
{
    Q_D(SystemInfo);
    d->_timeInterval = value;
    // 根据间隔时间重启定时器
    d->_timer->stop();
    if (value > 0)
    {
        // 立即推送一次之后在启动定时器
        d->_timer->start(value);
    }
}

CompleteCPUState SystemInfo::cpuState() const
{
    Q_D(const SystemInfo);
    return d->_interface->cpuState();
}

MemoryState SystemInfo::memoryState() const
{
    Q_D(const SystemInfo);
    return d->_interface->memoryState();
}

DiskState SystemInfo::diskState(const QString& dir) const
{
    Q_D(const SystemInfo);
    return d->_interface->diskState(dir);
}

void SystemInfo::clearPushDiskDir()
{
    Q_D(SystemInfo);
    d->_pushDiskDirList.clear();
}

void SystemInfo::addPushDiskDir(const QString& dir)
{
    Q_D(SystemInfo);
    d->_pushDiskDirList.push_back(dir);
}

void SystemInfo::addCPUAlterRate(double rate)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::CPURateIns().str()].push_back(rate);
}

void SystemInfo::addMemoryAlterRate(double rate)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::MemoryRateIns().str()].push_back(rate);
}

void SystemInfo::addMemoryAlterUsage(double usage)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::MemoryUsageIns().str()].push_back(usage);
}

void SystemInfo::addDiskAlterRate(const QString& dir, double rate)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::DiskRateIns().str(dir)].push_back(rate);
}

void SystemInfo::addDiskAlterUsage(const QString& dir, double usage)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::DiskUsageIns().str(dir)].push_back(usage);
}

void SystemInfo::removeCPUAlterRate(double rate)
{
    Q_D(SystemInfo);
    int pos = d->_alterThresholdMap[ThresholdType::CPURateIns().str()].indexOf(rate);
    if (pos < 0)
    {
        return ;
    }
    d->_alterThresholdMap[ThresholdType::CPURateIns().str()].removeAt(pos);
}

void SystemInfo::removeMemoryAlterRate(double rate)
{
    Q_D(SystemInfo);
    int pos = d->_alterThresholdMap[ThresholdType::MemoryRateIns().str()].indexOf(rate);
    if (pos < 0)
    {
        return ;
    }
    d->_alterThresholdMap[ThresholdType::MemoryRateIns().str()].removeAt(pos);
}

void SystemInfo::removeMemoryAlterUsage(double usage)
{
    Q_D(SystemInfo);
    int pos = d->_alterThresholdMap[ThresholdType::MemoryUsageIns().str()].indexOf(usage);
    if (pos < 0)
    {
        return ;
    }
    d->_alterThresholdMap[ThresholdType::MemoryUsageIns().str()].removeAt(pos);
}

void SystemInfo::removeDiskAlterRate(const QString& dir, double rate)
{
    Q_D(SystemInfo);
    int pos = d->_alterThresholdMap[ThresholdType::DiskRateIns().str(dir)].indexOf(rate);
    if (pos < 0)
    {
        return ;
    }
    d->_alterThresholdMap[ThresholdType::DiskRateIns().str()].removeAt(pos);
}

void SystemInfo::removeDiskAlterUsage(const QString& dir, double usage)
{
    Q_D(SystemInfo);
    int pos = d->_alterThresholdMap[ThresholdType::DiskUsageIns().str(dir)].indexOf(usage);
    if (pos < 0)
    {
        return ;
    }
    d->_alterThresholdMap[ThresholdType::DiskUsageIns().str()].removeAt(pos);
}

void SystemInfo::clearCPUAlterRate()
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::CPURateIns().str()].clear();
}

void SystemInfo::clearMemoryAlterRate()
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::MemoryRateIns().str()].clear();
}

void SystemInfo::clearMemoryAlterUsage()
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::MemoryUsageIns().str()].clear();
}

void SystemInfo::clearDiskAlterRate(const QString& dir)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::DiskRateIns().str(dir)].clear();
}

void SystemInfo::clearDiskAlterUsage(const QString& dir)
{
    Q_D(SystemInfo);
    d->_alterThresholdMap[ThresholdType::DiskUsageIns().str(dir)].clear();
}
