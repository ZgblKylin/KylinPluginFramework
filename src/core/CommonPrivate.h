#pragma once
#include <functional>
#include <Kpf/Kpf.h>

const QLoggingCategory& kpf();

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
#define defer Defer DEFER_TEMPNAME; DEFER_TEMPNAME =

template<typename T>
class NotifyManager : virtual public Kpf::INotifyManager<T>
{
public:
    virtual ~NotifyManager();

    virtual void registerNotifier(T* notifier) final;
    virtual void unregisterNotifier(T* notifier) final;

    template<typename Func, typename... Args>
    void notify(Func func, Args... args);

protected:
    QSet<T*> notifiers;
};

template<typename T>
NotifyManager<T>::~NotifyManager()
{
    qDeleteAll(notifiers);
}

template<typename T>
void NotifyManager<T>::registerNotifier(T* notifier)
{
    QMutexLocker locker(kpfMutex());
    notifiers.insert(notifier);
}

template<typename T>
void NotifyManager<T>::unregisterNotifier(T* notifier)
{
    QMutexLocker locker(kpfMutex());
    notifiers.remove(notifier);
}

template<typename T>
template<typename Func, typename... Args>
void NotifyManager<T>::notify(Func func, Args... args)
{
    for(T* notifier : notifiers)
    {
        (notifier->*func)(std::forward<Args>(args)...);
    }
}
