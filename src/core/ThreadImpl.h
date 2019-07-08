#ifndef THREADIMPL_H
#define THREADIMPL_H

#include <Kpf/Kpf.h>
#include "CommonPrivate.h"

namespace Kpf {
struct ObjectImpl;
struct EventBus;

struct ThreadImpl : public Thread
{
    virtual ~ThreadImpl();

    static QSharedPointer<ThreadImpl> create(const QString& name, bool externalThread = false);

    bool externalThread = false;

private:
    ThreadImpl(bool externalThread);
};

class ThreadManagerImpl : public ThreadManager, public NotifyManager<IThreadNotifier>
{
    Q_OBJECT

public:
    ThreadManagerImpl();
    virtual ~ThreadManagerImpl();

    static ThreadManagerImpl& instance();

    // ThreadManager interface
    virtual QStringList threadNames() const override;
    virtual QWeakPointer<Thread> findThread(const QString& threadName) const override;
    virtual QWeakPointer<Thread> defaultThread() const override;
    virtual void removeThread(const QWeakPointer<Thread>& thread) override;
    virtual void registerNotifier(IThreadNotifier* notifier) override;
    virtual void unregisterNotifier(IThreadNotifier* notifier) override;

    void setObjectThread(QSharedPointer<ObjectImpl>& object);

private:
    QStack<QString> names;
    QMap<QString, QSharedPointer<ThreadImpl>> threads;
    QSharedPointer<ThreadImpl> mainThread;
};
} // namespace Kpf
#define kpfThreadImpl Kpf::ThreadManagerImpl::instance()

#endif // THREADIMPL_H
