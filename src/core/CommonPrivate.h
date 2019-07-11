#pragma once
#include <Kpf/Kpf.h>
#include <QtXml/QtXml>
#include <functional>

QString normalizedSignature(QString signature);
QByteArray convertSignalName(QByteArray signal);
QByteArray convertSlotName(QByteArray slot);

QMutex* kpfMutex();

template<typename T>
using Ptr = T*;
template<typename T>
using Ref = T&;

class Defer
{
public:
    Defer() = default;
    Defer(std::function<void(void)> onDelete) : onDel(onDelete) {}
    ~Defer() { if (onDel) { onDel(); } }

    Defer(Defer&& other) : onDel(other.onDel) {}
    Defer& operator=(Defer&& other) { onDel = other.onDel; return *this; }
    Defer& operator=(std::function<void(void)> onDelete) { onDel = onDelete; return *this; }
    void reset() { onDel = {}; }

private:
    Defer(const Defer&) = delete;
    Defer& operator=(const Defer&) = delete;
    std::function<void(void)> onDel;
};
#define DEFER_STRCAT_2(A, B) A ## B
#define DEFER_STRCAT(A, B) DEFER_STRCAT_2(A, B)
#define DEFER_TEMPNAME DEFER_STRCAT(__defer, __LINE__)

template<typename T>
class NotifyManagerImpl : virtual public Kpf::NotifyManager<T>
{
public:
    virtual ~NotifyManagerImpl();

    virtual void registerNotifier(T* notifier) override;
    virtual void unregisterNotifier(T* notifier) override;

    template<typename Func, typename... Args>
    void notify(Func func, Args... args);

protected:
    QList<T*> notifiers;
};

template<typename T>
NotifyManagerImpl<T>::~NotifyManagerImpl()
{
    qDeleteAll(notifiers);
}

template<typename T>
void NotifyManagerImpl<T>::registerNotifier(T* notifier)
{
    QMutexLocker locker(kpfMutex());
    if (!notifiers.contains(notifier)) {
        notifiers << notifier;
    }
}

template<typename T>
void NotifyManagerImpl<T>::unregisterNotifier(T* notifier)
{
    QMutexLocker locker(kpfMutex());
    notifiers.removeAll(notifier);
}

template<typename T>
template<typename Func, typename... Args>
void NotifyManagerImpl<T>::notify(Func func, Args... args)
{
    for(T* notifier : notifiers)
    {
        (notifier->*func)(std::forward<Args>(args)...);
    }
}
