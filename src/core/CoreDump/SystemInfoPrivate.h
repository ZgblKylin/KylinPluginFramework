#pragma once
#include "SystemInfoInterface.h"
#include <QVector>
#include <QMap>
#include <functional>

class ThresholdType
{
public:
#define DeclareIns(Type) \
    static ThresholdType Type##Ins() \
    { \
        return ThresholdType(Type); \
    }

    DeclareIns(CPURate)
    DeclareIns(MemoryRate)
    DeclareIns(MemoryUsage)
    DeclareIns(DiskRate)
    DeclareIns(DiskUsage)

    // 只有针对磁盘的类型信息，data才有效
    QString str(const QString& data = QString()) const
    {
        switch (t)
        {
        case CPURate:
            return QString("^CPURate^");
        case MemoryRate:
            return QString("^MemoryRate^");
        case MemoryUsage:
            return QString("^MemoryUsage^");
        case DiskRate:
            return QString("^DiskRate^%1").arg(data);
        case DiskUsage:
            return QString("^DiskUsage^%1").arg(data);
        default:
            break;
        }
        return data;
    }

private:
    enum T
    {
        CPURate,
        MemoryRate,
        MemoryUsage,
        DiskRate,
        DiskUsage,
    }t;

    ThresholdType(T _t)
        : t(_t)
    {
    }
};

class QTimer;
class SystemInfo;
class SystemInfoPrivate
{
public:
    explicit SystemInfoPrivate(SystemInfo* pthis);
    ~SystemInfoPrivate();

public:
    SystemInfo* q_ptr;
    Q_DECLARE_PUBLIC(SystemInfo)

public:
    SystemInfoInterface* _interface; //< 接口的实例

    AutoPush::Flag _autoPush = AutoPush::None; //< 状态定时推送标记
    int _timeInterval = -1; //< 推送状态的时间间隔

    QVector<QString> _pushDiskDirList; //< 推送状态的磁盘目录

    QTimer* _timer = nullptr; //< 状态推送的定时器

    QMap<QString, QVector<double>> _alterThresholdMap; //< 报警阈值Map
    QMap<QString, double> _lastValue; //< 最后一次的值

public:
    /**
     * @brief 推送系统信息
     */
    void pushSystemInfo();

    /**
     * @brief 推送CPU使用率的阈值
     * @param rate
     */
    void pushCPURateThreshol(double rate);

    /**
     * @brief 推送内存使用率的阈值
     */
    void pushMemoryRateThreshol(double rate);

    /**
     * @brief 推送内存使用量的阈值
     */
    void pushMemoryUsageThreshol(double usage);

    /**
     * @brief 推送磁盘使用率的阈值
     */
    void pushDiskRateThreshol(const QString& dir, double rate);

    /**
     * @brief 推送磁盘使用量的阈值
     */
    void pushDiskUsageThreshol(const QString& dir, double usage);

    /**
     * @brief 检测新的需要推送的阈值
     * @param newValue
     * @param threshols
     * @return 是警告还是警告清除
     */
    bool checkPushThreshol(const QString& key, double newValue,
                           QVector<double>& threshols);
};
