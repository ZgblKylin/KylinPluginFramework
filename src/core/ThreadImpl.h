#pragma once
#include <Kpf/Kpf.h>
#include "CommonPrivate.h"

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

namespace Kpf {
struct ObjectImpl;
class EventBus;

struct ThreadImpl : public Thread
{
    virtual ~ThreadImpl() override;

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
    virtual ~ThreadManagerImpl() override;

    static ThreadManagerImpl& instance();

    // ThreadManager interface
    virtual QStringList threadNames() const override;
    virtual QWeakPointer<Thread> findThread(const QString& threadName) const override;
    virtual QWeakPointer<Thread> defaultThread() const override;
    virtual void removeThread(const QWeakPointer<Thread>& thread) override;

    void setObjectThread(QSharedPointer<ObjectImpl>& object);

private:
    QStack<QString> names;
    QMap<QString, QSharedPointer<ThreadImpl>> threads;
    QSharedPointer<ThreadImpl> mainThread;
};
} // namespace Kpf
#define kpfThreadImpl Kpf::ThreadManagerImpl::instance()

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
