#pragma once
#include "SystemInfoType.h"
#include <QtCore/QObject>

class SystemInfoPrivate;
class SystemInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(AutoPushFlag)

public:
    // 自动推送标记
    Q_PROPERTY(AutoPush::Flag autoPushFlag READ autoPushFlag WRITE setAutoPushFlag)
    // 自动推送间隔时间
    Q_PROPERTY(int timeInterval READ timeInterval WRITE setTimeInterval)

public:
    SystemInfo(QObject* parent = nullptr);
    ~SystemInfo();

public:
    /******************************************************
     *  属性值的 getter and setter 接口
     ******************************************************/

    /**
     * @brief autoPushFlag
     * @return
     */
    AutoPush::Flag autoPushFlag() const;

    /**
     * @brief timeInterval
     * @return
     */
    int timeInterval() const;

    /**
     * @brief setAutoPushFlag
     * @param value
     */
    void setAutoPushFlag(AutoPush::Flag value);

    /**
     * @brief setTimeInterval
     * @param value
     */
    void setTimeInterval(int value);

public:
    /******************************************************
     *  公开的其他接口
     ******************************************************/

    /**
     * @brief 获取CPU的的状态信息
     * @return
     */
    CompleteCPUState cpuState() const;

    /**
     * @brief 获取内存的状态
     * @return
     */
    MemoryState memoryState() const;

    /**
     * @brief 获取磁盘指定目录的状态
     * @return
     */
    DiskState diskState(const QString& dir) const;

    /**
     * @brief 清除推送的磁盘目录
     */
    void clearPushDiskDir();

    /**
     * @brief 增加推送的磁盘目录
     * @param dir
     */
    void addPushDiskDir(const QString& dir);

    /**
     * @brief 增加cpu检测阈值
     * @param rate 使用率
     */
    void addCPUAlterRate(double rate);

    /**
     * @brief 增加内存检测阈值（使用率）
     * @param rate 使用率
     */
    void addMemoryAlterRate(double rate);

    /**
     * @brief 增加内存检测阈值（使用量）
     * @param usage 使用量
     */
    void addMemoryAlterUsage(double usage);

    /**
     * @brief 增加磁盘检测阈值（使用率）
     * @param dir
     * @param threshold
     */
    void addDiskAlterRate(const QString& dir, double rate);

    /**
     * @brief 增加磁盘检测阈值（使用量）
     * @param dir
     * @param usage
     */
    void addDiskAlterUsage(const QString& dir, double usage);

    /**
     * @brief 删除cpu检测阈值
     * @param rate
     */
    void removeCPUAlterRate(double rate);

    /**
     * @brief 删除内存检测阈值（使用率）
     * @param rate
     */
    void removeMemoryAlterRate(double rate);

    /**
     * @brief 删除内存检测阈值（使用量）
     * @param usage
     */
    void removeMemoryAlterUsage(double usage);

    /**
     * @brief 删除磁盘检测阈值（使用量）
     * @param dir
     * @param usage
     */
    void removeDiskAlterUsage(const QString& dir, double usage);

    /**
     * @brief 删除磁盘检测阈值（使用率）
     * @param dir
     * @param rate
     */
    void removeDiskAlterRate(const QString& dir, double rate);

    /**
     * @brief 清空cpu检测阈值
     * @param rate
     */
    void clearCPUAlterRate();

    /**
     * @brief 清空内存检测阈值（使用率）
     */
    void clearMemoryAlterRate();

    /**
     * @brief 清空内存检测阈值（使用量）
     */
    void clearMemoryAlterUsage();

    /**
     * @brief 清空磁盘检测阈值（使用率）
     * @param dir
     */
    void clearDiskAlterRate(const QString& dir);

    /**
     * @brief 清空磁盘检测阈值（使用量）
     * @param dir
     */
    void clearDiskAlterUsage(const QString& dir);

signals:
    /**
     * @brief pushCPUUsageRate 推送CPU的使用率
     * @param coreId
     * @param ratio
     */
    void pushCPUUsageRate(int coreId, double ratio);

    /**
     * @brief 推送内存的使用量
     * @param remain 单位为 GB
     */
    void pushMemoryUsage(double usage);

    /**
     * @brief 推送内存的使用率
     * @param rate
     */
    void pushMemoryUsageRate(double rate);

    /**
     * @brief 推送内存的剩余量
     * @param rate 单位为 GB
     */
    void pushMemoryRemain(double remain);

    /**
     * @brief 推送内存的剩余率
     * @param rate
     */
    void pushMemoryRemainRate(double rate);

    /**
     * @brief 推送磁盘的使用量
     * @param remain 单位为 GB
     */
    void pushDiskUsage(const QString& dir, double usage);

    /**
     * @brief 推送磁盘的使用率
     * @param rate
     */
    void pushDiskUsageRate(const QString& dir, double rate);

    /**
     * @brief 推送磁盘的剩余量
     * @param rate 单位为 GB
     */
    void pushDiskRemain(const QString& dir, double remain);

    /**
     * @brief 推送磁盘的剩余率
     * @param rate
     */
    void pushDiskRemainRate(const QString& dir, double rate);

    /**
     * @brief 推送CPU使用率报警
     * @param rate
     */
    void pushCPURateAlter(double rate);

    /**
     * @brief 推送CPU使用率报警清除
     * @param rate
     */
    void pushCPURateAlterClear(double rate);

    /**
     * @brief 推送内存使用率报警
     * @param rate
     */
    void pushMemoryRateAlter(double rate);

    /**
     * @brief 推送内存使用率报警清除
     * @param rate
     */
    void pushMemoryRateAlterClear(double rate);

    /**
     * @brief 推送内存使用量报警
     * @param rate
     */
    void pushMemoryUsageAlter(double usage);

    /**
     * @brief 推送内存使用量报警清除
     * @param rate
     */
    void pushMemoryUsageAlterClear(double usage);

    /**
     * @brief 推送磁盘使用率报警
     * @param dir
     * @param rate
     */
    void pushDiskRateAlter(const QString& dir, double rate);

    /**
     * @brief 推送磁盘使用率报警清除
     * @param dir
     * @param rate
     */
    void pushDiskRateAlterClear(const QString& dir, double rate);

    /**
     * @brief 推送磁盘使用量报警
     * @param dir
     * @param rate
     */
    void pushDiskUsageAlter(const QString& dir, double usage);

    /**
     * @brief 推送磁盘使用量报警清除
     * @param dir
     * @param rate
     */
    void pushDiskUsageAlterClear(const QString& dir, double usage);

private:
    Q_DISABLE_COPY(SystemInfo)
    Q_DECLARE_PRIVATE(SystemInfo)
    QScopedPointer<SystemInfoPrivate> d_ptr;
};
