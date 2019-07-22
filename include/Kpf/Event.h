#pragma once
#include <Kpf/Common.h>

// ======== API声明 ========

// 获取事件管理器对象，等同于Kpf::EventManager::instance()
#define kpfEvent

namespace Kpf {
struct Topic;
struct ObjectEvent;
struct Object;

struct KPFSHARED_EXPORT MetaEvent
{
    enum Type { Publish, Subscribe };
    virtual ~MetaEvent();

    Type type;
    QString name;
    QString className;
    QSet<ObjectEvent*> eventObjects;

protected:
    MetaEvent();
};

struct KPFSHARED_EXPORT ObjectEvent
{
    virtual ~ObjectEvent();

    QWeakPointer<MetaEvent> event;
    QMap<QString, QWeakPointer<Topic>> topics;

    QWeakPointer<Object> object;
    std::function<QVariant(const QVariantList&)> func;

    QMap<QString, QVariant> send(const QVariantList& args);
    void post(const QVariantList& args);
};

struct IEventNotifier
{
    virtual ~IEventNotifier() = default;

    virtual void pubEventRegistered(const QString& eventName) { Q_UNUSED(eventName) }
    virtual void subEventRegistered(const QString& eventName) { Q_UNUSED(eventName) }

    virtual void pubEventAdded(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignal = false) { Q_UNUSED(objectName) Q_UNUSED(eventName) Q_UNUSED(topicName) Q_UNUSED(isSignal) }
    virtual void subEventAdded(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignalOrSlot = false) { Q_UNUSED(objectName) Q_UNUSED(eventName) Q_UNUSED(topicName) Q_UNUSED(isSignalOrSlot) }

    virtual void sendEvent(const ObjectEvent& event, const QVariantList& args) { Q_UNUSED(event) Q_UNUSED(args) }
    virtual void postEvent(const ObjectEvent& event, const QVariantList& args) { Q_UNUSED(event) Q_UNUSED(args) }
};

class KPFSHARED_EXPORT EventManager : public QObject, virtual public INotifyManager<IEventNotifier>
{
    Q_OBJECT
public:
    virtual ~EventManager() = default;

    static EventManager& instance();

    /**
     * @brief addPubEvent 添加事件发布
     * @param objectName 发布事件的对象名
     * @param event 事件标识
     * @param topic 事件的目标主题
     */
    virtual void addPubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignal = false) = 0;
    /**
     * @brief addSubEvent 添加事件订阅
     * @param objectName 订阅事件的对象名
     * @param event 事件标识
     * @param topic 事件的来源主题
     */
    virtual void addSubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignalOrSlot = false) = 0;

    virtual void registerPubEvent(const QString& eventName, ObjectEvent* eventObject) = 0;
    virtual void registerSubEvent(const QString& eventName, ObjectEvent* eventObject) = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfEvent
#define kpfEvent Kpf::EventManager::instance()
