#include "ThreadImpl.h"
#include "KpfPrivate.h"

Kpf::Thread::~Thread()
{
}

Kpf::Thread::Thread()
{
}

Kpf::ThreadImpl::~ThreadImpl()
{
    if (kpfCoreImpl.closingDown()) {
        delete eventBus;
    } else {
        eventBus->deleteLater();
    }

    if (externalThread) {
        return;
    }
    thread->requestInterruption();
    thread->quit();
    thread->wait();
    if (kpfCoreImpl.closingDown()) {
        delete thread;
    } else {
        thread->deleteLater();
    }
    kpfThreadImpl.notify(&IThreadNotifier::threadStopped, name);
}

QSharedPointer<Kpf::ThreadImpl> Kpf::ThreadImpl::create(const QString& name, bool externalThread)
{
    QSharedPointer<ThreadImpl> ret(new ThreadImpl(externalThread));
    ret->name = name;
    return ret;
}

Kpf::ThreadImpl::ThreadImpl(bool external)
{
    eventBus = new EventBus;

    if (external)
    {
        externalThread = true;
    }
    else
    {
        thread = new QThread;
        thread->setObjectName(name);
        thread->start();
        eventBus->moveToThread(thread);
        kpfThreadImpl.notify(&IThreadNotifier::threadStarted, name);
    }
}

Kpf::ThreadManager& Kpf::ThreadManager::instance()
{
    return kpfCoreImpl.threadManager();
}

Kpf::ThreadManagerImpl::ThreadManagerImpl()
    : mainThread(ThreadImpl::create("", true))
{
}

Kpf::ThreadManagerImpl::~ThreadManagerImpl()
{
    QMutexLocker locker(kpfMutex());
    while (!names.isEmpty()) {
        threads.remove(names.pop());
    }
}

Kpf::ThreadManagerImpl &Kpf::ThreadManagerImpl::instance()
{
    return kpfCoreImpl.threadManager();
}

QStringList Kpf::ThreadManagerImpl::threadNames() const
{
    QMutexLocker locker(kpfMutex());
    return threads.keys();
}

QWeakPointer<Kpf::Thread> Kpf::ThreadManagerImpl::findThread(const QString& threadName) const
{
    QMutexLocker locker(kpfMutex());
    return threads.value(threadName).staticCast<Thread>();
}

QWeakPointer<Kpf::Thread> Kpf::ThreadManagerImpl::defaultThread() const
{
    return mainThread;
}

void Kpf::ThreadManagerImpl::removeThread(const QWeakPointer<Thread>& t)
{
    QMutexLocker locker(kpfMutex());
    QSharedPointer<Thread> thread = t.toStrongRef();
    if (thread) {
        threads.remove(thread->name);
    }
}

void Kpf::ThreadManagerImpl::setObjectThread(QSharedPointer<ObjectImpl>& object)
{
    QMutexLocker locker(kpfMutex());

    QSharedPointer<ThreadImpl> thread = mainThread;

    bool subThread = object->config.attribute(KEY_SUBTHREAD).toLower() == "true";
    if (subThread)
    {
        QString threadName = object->config.attribute(KEY_THREADNAME);
        thread = kpfThread.findThread(threadName)
                 .toStrongRef().staticCast<ThreadImpl>();
        if (!thread)
        {
            thread = ThreadImpl::create(threadName);
            names.push(threadName);
            threads.insert(threadName, thread);
        }

        object->object->setParent(nullptr);
        object->object->moveToThread(thread->thread);

        qCDebug(kpf) << "Move object" << object->name
                     << "to thread" << threadName;
    }

    object->thread = thread;
    thread->objects << object.data();
}
