#pragma once
#include <Kpf/Common.h>
#include <Kpf/Constants.h>
#include <Kpf/Connection.h>
#include <Kpf/Thread.h>
#include <Kpf/Object.h>
#include <Kpf/Event.h>
#include <Kpf/Topic.h>

// ======== API声明 ========

// 获取框架内核对象，等同于Kpf::KpfCore::instance()
#define kpfCore

namespace Kpf {
struct ICoreNotifier
{
    virtual ~ICoreNotifier() = default;

    virtual void beginInitialization() {}
    virtual void initializationFinished() {}

    virtual void loggerInitialized() {}

    // Plugins initialization
    virtual void beginLoadLibrary(const QFileInfo& library) { Q_UNUSED(library) }
    virtual void libraryLoaded(const QFileInfo& library) { Q_UNUSED(library) }
    virtual void libraryUnloaded(const QFileInfo& library) { Q_UNUSED(library) }

    // Config initialization
    virtual void componentLoaded(const QFileInfo& componentFile) { Q_UNUSED(componentFile) }
    virtual void appConfigLoaded() {}
    virtual void componentsExpanded() {}

    virtual void aboutToQuit() {}
    virtual void connectionManagerDestroyed() {}
    virtual void topicManagerDestroyed() {}
    virtual void eventManagerDestroyed() {}
    virtual void objectManagerDestroyed() {}
    virtual void threadManagerDestroyed() {}
    virtual void classManagerDestroyed() {}
    virtual void allLibraryUnloaded() {}
    virtual void quitFinished() {}
};

/**
 * @brief KpfCore类，KPF框架内核API声明
 */
class KPFSHARED_EXPORT KpfCore : virtual public INotifyManager<ICoreNotifier>
{
public:
    virtual ~KpfCore() = default;

    // 获取内核对象
    static KpfCore& instance();

    // 初始化内核，加载配置文件，需要在main函数中调用
    virtual bool init(int argc, char *argv[]) = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfCore
#define kpfCore ::Kpf::KpfCore::instance()
