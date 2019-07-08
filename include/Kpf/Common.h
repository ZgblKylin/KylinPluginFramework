/* Common.h
 * 提供常用头文件包含和一些工具类
 */
#ifndef KPF_COMMON_H
#define KPF_COMMON_H

#include <climits>
#include <cstdint>
#include <type_traits>
#include <functional>
#include <QtCore/QtCore>

#if defined(KPF_LIBRARY)
#  define KPFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KPFSHARED_EXPORT Q_DECL_IMPORT
#endif

template<typename T>
using Ptr = T*;
template<typename T>
using Ref = T&;

/**
 * @brief RAII类，提供自动资源回收，在栈上构造对象，退出作用于时会自动执行onDelete回调
 * 范例：
 * QFile file = new QFile(fileName);
 * RAII raii([file]{ file->flush(); file->close(); file->deleteLater(); });
 *
 * file.open(QFile::WriteOnly);
 * ...
 * return; // 此处会自动执行onDelete
 */
class RAII
{
    std::function<void(void)> onDel;
public:
    RAII(const std::function<void(void)>& onDelete) : onDel(onDelete) {}
    ~RAII() { onDel(); }
};

// 简易化QReadWriteLock，声明为普通成员对象即可。禁止声明为mutable，否则无法进行读锁定
class ReadWriteLock : private QReadWriteLock
{
public:
    ReadWriteLock(QReadWriteLock::RecursionMode recursionMode = QReadWriteLock::NonRecursive)
        : QReadWriteLock(recursionMode) {}
    ~ReadWriteLock() = default;

    // 非const方法中调用，则执行lockForWrite
    ReadWriteLock* lock() { lockForWrite(); return this; }

    // const方法中调用，则执行lockForRead
    const ReadWriteLock* lock() const
    {
        ReadWriteLock* self = const_cast<ReadWriteLock*>(this);
        self->lockForRead();
        return self;
    }

    void unlock() const { static_cast<QReadWriteLock*>(const_cast<ReadWriteLock*>(this))->unlock(); }
};
// 简易化Locker，无需区分QReadLocker或QWriteLocker，仅需声明ReadWriteLockLocker locker(lock->lock());
class ReadWriteLocker
{
public:
    explicit ReadWriteLocker(ReadWriteLock* lock) : l(lock) {}
    explicit ReadWriteLocker(const ReadWriteLock* lock) : l(lock) {}
    ~ReadWriteLocker() { l->unlock(); }
private:
    const ReadWriteLock* l;
};

namespace Kpf {
template<typename T, typename B, bool baseIsBase = std::is_base_of<B, T>::value>
struct ToDelivered;
template<typename T, typename B>
struct ToDelivered<T, B, true>
{
    T* delivered(B* base)
    {
        return dynamic_cast<T*>(base);
    }
};
template<typename T, typename B>
struct ToDelivered<T, B, false>
{
    T* delivered(B*)
    {
        return nullptr;
    }
};
template<typename T, typename B>
T* toDelivered(B* base)
{
    return ToDelivered<T, B>().delivered(base);
}

template<typename T, typename D, bool baseIsBase = std::is_base_of<T, D>::value>
struct ToBase;
template<typename T, typename D>
struct ToBase<T, D, true>
{
    T* base(D* delivered)
    {
        return static_cast<T*>(delivered);
    }
};
template<typename T, typename D>
struct ToBase<T, D, false>
{
    T* base(D*)
    {
        return nullptr;
    }
};
template<typename T, typename D>
T* toBase(D* delivered)
{
    return ToBase<T, D>().base(delivered);
}
} // namesapce Kpf

#endif // KPF_COMMON_H
