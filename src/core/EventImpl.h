#pragma once
#include <Kpf/Kpf.h>
#include "CommonPrivate.h"
#include "ObjectImpl.h"

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

namespace Kpf {
struct MetaEventImpl : public MetaEvent
{
    ~MetaEventImpl() override;

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
    virtual ~EventManagerImpl() override;

    static EventManagerImpl& instance();

    // EventManager interface
    virtual void addPubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignal = false) override;
    virtual void addSubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignalOrSlot = false) override;
    virtual void registerPubEvent(const QString& eventName, ObjectEvent* eventObject) override;
    virtual void registerSubEvent(const QString& eventName, ObjectEvent* eventObject) override;

    void setupObjectEvents(QSharedPointer<ObjectImpl>& object);
    void registerPubSignalEvent(QSharedPointer<ObjectImpl>& object, const QString& eventName);
    void registerSubSignalSlotEvent(QSharedPointer<ObjectImpl>& object, const QString& eventName);
    void registerEvent(const QString& eventName, ObjectEvent* eventObject, MetaEvent::Type type, QSharedPointer<Kpf::ObjectImpl> object = {});
};
} // namespace Kpf
#define kpfEventImpl Kpf::EventManagerImpl::instance()

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
