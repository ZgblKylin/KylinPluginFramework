#ifndef COMMONPRIVATE_H
#define COMMONPRIVATE_H

#include <Kpf/Kpf.h>

QString normalizedSignature(QString signature);
QByteArray convertSignalName(QByteArray signal);
QByteArray convertSlotName(QByteArray slot);

QMutex* kpfMutex();

template<typename T>
class NotifyManager
{
public:
    using N = T;

    virtual ~NotifyManager();

    void registerNotifier(T* notifier);
    void unregisterNotifier(T* notifier);

    template<typename Func, typename... Args>
    void notify(Func func, Args... args);

protected:
    QList<T*> notifiers;
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
    if (!notifiers.contains(notifier)) {
        notifiers << notifier;
    }
}

template<typename T>
void NotifyManager<T>::unregisterNotifier(T* notifier)
{
    QMutexLocker locker(kpfMutex());
    notifiers.removeAll(notifier);
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

#endif // COMMONPRIVATE_H
