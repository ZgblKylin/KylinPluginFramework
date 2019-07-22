#pragma once
#include <type_traits>
#include <QtCore/QtCore>
#include <QtXml/QtXml>

#if defined(KPF_LIBRARY)
#  define KPFSHARED_EXPORT Q_DECL_EXPORT
#else
#  define KPFSHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Kpf {
template<typename T>
class INotifyManager
{
public:
    using N = T;

    virtual ~INotifyManager() = default;

    // 注册监听器
    // 注册后，监听器所有权会转移至框架，由框架负责释放
    // 取消注册后，监听器所有权返还用户
    virtual void registerNotifier(T* notifier) = 0;
    virtual void unregisterNotifier(T* notifier) = 0;
};

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

Q_DECLARE_METATYPE(QDomElement)
