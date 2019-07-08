#ifndef EVENTIMPL_H
#define EVENTIMPL_H

#include <Kpf/Kpf.h>
#include "CommonPrivate.h"
#include "ObjectImpl.h"

namespace Kpf {
struct MetaEventImpl : public MetaEvent
{
    ~MetaEventImpl();

    static QSharedPointer<MetaEventImpl> create(Type type,
                                            const QString& name,
                                            const QString& className);
private:
    MetaEventImpl();
};

class EventManagerImpl : public EventManager, public NotifyManager<IEventNotifier>
{
    Q_OBJECT
public:
    EventManagerImpl();
    virtual ~EventManagerImpl();

    static EventManagerImpl& instance();

    // EventManager interface
    virtual void addPubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignal = false) override;
    virtual void addSubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignalOrSlot = false) override;
    virtual void registerPubEvent(const QString& eventName, ObjectEvent* eventObject) override;
    virtual void registerSubEvent(const QString& eventName, ObjectEvent* eventObject) override;
    virtual void registerNotifier(IEventNotifier* notifier) override;
    virtual void unregisterNotifier(IEventNotifier* notifier) override;

    void setupObjectEvents(QSharedPointer<ObjectImpl>& object);
    void registerPubSignalEvent(QSharedPointer<ObjectImpl>& object, const QString& eventName);
    void registerSubSignalSlotEvent(QSharedPointer<ObjectImpl>& object, const QString& eventName);
    void registerEvent(const QString& eventName, ObjectEvent* eventObject, MetaEvent::Type type, QSharedPointer<Kpf::ObjectImpl> object = {});
};
} // namespace Kpf
#define kpfEventImpl Kpf::EventManagerImpl::instance()

#endif // EVENTIMPL_H
