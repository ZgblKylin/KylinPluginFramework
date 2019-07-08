#ifndef KPF_THREAD_H
#define KPF_THREAD_H

#include <Kpf/Common.h>

// ======== API声明 ========

// 获取ThreadManager对象，等同于Kpf::ThreadManager::instance()
#define kpfThread

namespace Kpf {
struct Object;

struct KPFSHARED_EXPORT Thread
{
    virtual ~Thread();

    QThread* thread;
    QString name;

    QObject* eventBus;
    QSet<Object*> objects;

protected:
    Thread();
};

struct IThreadNotifier
{
    virtual ~IThreadNotifier() = default;

    virtual void threadStarted(const QString& threadName) { Q_UNUSED(threadName) }
    virtual void threadStopped(const QString& threadName) { Q_UNUSED(threadName) }
};

class KPFSHARED_EXPORT ThreadManager : public QObject
{
    Q_OBJECT
public:
    virtual ~ThreadManager() = default;

    static ThreadManager& instance();

    virtual QStringList threadNames() const = 0;
    virtual QWeakPointer<Thread> findThread(const QString& threadName) const = 0;
    virtual QWeakPointer<Thread> defaultThread() const = 0;
    virtual void removeThread(const QWeakPointer<Thread>& thread) = 0;

    // 注册监听器
    // 注册后，监听器所有权会转移至框架，由框架负责释放
    // 取消注册后，监听器所有权返还用户
    virtual void registerNotifier(IThreadNotifier* notifier) = 0;
    virtual void unregisterNotifier(IThreadNotifier* notifier) = 0;
};
} // namesapace Kpf
// ======== API声明 ========

#undef kpfThread
#define kpfThread Kpf::ThreadManager::instance()

#endif // KPF_THREAD_H
