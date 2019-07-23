#include "EventImpl.h"
#include "KpfPrivate.h"

Kpf::MetaEvent::~MetaEvent()
{
}

Kpf::MetaEvent::MetaEvent()
{
}

Kpf::ObjectEvent::~ObjectEvent()
{
    QSharedPointer<MetaEvent> e = event.toStrongRef();
    if (e) {
        e->eventObjects.remove(this);
    }

    for (auto it = topics.begin(); it != topics.end(); ++it)
    {
        QSharedPointer<Topic> topic = it->toStrongRef();
        if (!topic) {
            continue;
        }

        topic->publishedEvents.remove(this);
        topic->subscribedEvents.remove(this);

        for (auto threadEvents = topic->threadEvents.begin();
             threadEvents != topic->threadEvents.end();
             ++threadEvents)
        {
            threadEvents->removeAll(this);
        }
    }
}

QMap<QString, QVariant> Kpf::ObjectEvent::send(const QVariantList& args)
{
    QMap<QString, QVariant> ret;
    if (kpfCoreImpl.closingDown()) return ret;

    for (auto it = topics.begin(); it != topics.end(); ++it)
    {
        QSharedPointer<Topic> topic = it.value().toStrongRef();
        if (!topic) {
            continue;
        }

        for (ObjectEvent* subscribedEvent : topic->subscribedEvents)
        {
            ret[subscribedEvent->object.toStrongRef()->name]
                    = subscribedEvent->func(args);
        }
    }

    kpfEventImpl.notify(&IEventNotifier::sendEvent, *this, args);
    return ret;
}

void Kpf::ObjectEvent::post(const QVariantList& args)
{
    QMap<QString, QVariantList> ret;
    if (kpfCoreImpl.closingDown()) {
        return;
    }

    for (auto it = topics.begin(); it != topics.end(); ++it)
    {
        QSharedPointer<Topic> topic = it.value().toStrongRef();
        if (!topic) {
            continue;
        }

        for (auto it = topic->threadEvents.begin();
             it != topic->threadEvents.end();
             ++it)
        {
            ThreadImpl* thread;
            thread = static_cast<ThreadImpl*>(reinterpret_cast<Thread*>(it.key()));
            PostEvent* event = new PostEvent(args);
            event->eventObjects = it.value();
            qApp->postEvent(thread->eventBus,
                            event,
                            Qt::HighEventPriority);
        }
    }

    kpfEventImpl.notify(&IEventNotifier::postEvent, *this, args);
}

Kpf::MetaEventImpl::~MetaEventImpl()
{
}

QSharedPointer<Kpf::MetaEventImpl> Kpf::MetaEventImpl::create(Type type, const QString& name, const QString& className)
{
    QSharedPointer<MetaEventImpl> ret(new MetaEventImpl);
    ret->type = type;
    ret->name = name;
    ret->className = className;
    return ret;
}

Kpf::MetaEventImpl::MetaEventImpl()
{
}

Kpf::EventManager& Kpf::EventManager::instance()
{
    return kpfCoreImpl.eventManager();
}

Kpf::EventManagerImpl::EventManagerImpl()
{
}

Kpf::EventManagerImpl::~EventManagerImpl()
{
    QMutexLocker locker(kpfMutex());
}

Kpf::EventManagerImpl& Kpf::EventManagerImpl::instance()
{
    return kpfCoreImpl.eventManager();
}

void Kpf::EventManagerImpl::addPubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignal)
{
    QMutexLocker locker(kpfMutex());

    QSharedPointer<ObjectImpl> object = kpfObject.findObject(objectName)
                                        .toStrongRef()
                                        .staticCast<ObjectImpl>();
    if (!object) {
        return;
    }

    QString name = eventName;
    if (isSignal)
    {
        name = normalizedSignature(name);
        registerPubSignalEvent(object, name);
    }
    ObjectEvent* eventObject = object->publishedEvents.value(name);
    if (!eventObject)
    {
        qCWarning(kpf) << "Add published event for object" << object->name
                       << "wtih event signature" << name
                       << "to topic" << topicName
                       << "failed : cannot find corresponding event";
        return;
    }

    qCDebug(kpf) << "Add published event for object" << object->name
                 << "wtih event signature" << name
                 << "to topic" << topicName;

    QSharedPointer<TopicImpl> topic = kpfTopicImpl.addTopic(topicName);
    topic->publishedEvents.insert(eventObject);
    eventObject->topics.insert(topic->name, topic);
    notify(&N::pubEventAdded, object->name, name, topicName, isSignal);
}

void Kpf::EventManagerImpl::addSubEvent(const QString& objectName, const QString& eventName, const QString& topicName, bool isSignalOrSlot)
{
    QMutexLocker locker(kpfMutex());

    QSharedPointer<ObjectImpl> object = kpfObject.findObject(objectName)
                                        .toStrongRef()
                                        .staticCast<ObjectImpl>();
    if (!object) {
        return;
    }

    QString name = eventName;
    if (isSignalOrSlot)
    {
        name = normalizedSignature(name);
        registerSubSignalSlotEvent(object, name);
    }

    ObjectEvent* eventObject = object->subscribedEvents.value(name);
    if (!eventObject)
    {
        qCWarning(kpf) << "Add subscribed event for object" << object->name
                       << "wtih event signature" << name
                       << "to topic" << topicName
                       << "failed : cannot find corresponding event";
        return;
    }

    qCDebug(kpf) << "Add subscribed event for object" << object->name
                 << "wtih event signature" << name
                 << "to topic" << topicName;

    QSharedPointer<TopicImpl> topic = kpfTopicImpl.addTopic(topicName);
    topic->subscribedEvents.insert(eventObject);
    topic->threadEvents[reinterpret_cast<quintptr>(object->thread.data())].append(eventObject);

    eventObject->topics.insert(topic->name, topic);

    notify(&N::subEventAdded, object->name, name, topicName, isSignalOrSlot);
}

void Kpf::EventManagerImpl::registerPubEvent(const QString& eventName, ObjectEvent* eventObject)
{
    QMutexLocker locker(kpfMutex());
    registerEvent(eventName, eventObject, MetaEvent::Publish);
}

void Kpf::EventManagerImpl::registerSubEvent(const QString& eventName, ObjectEvent* eventObject)
{
    QMutexLocker locker(kpfMutex());
    registerEvent(eventName, eventObject, MetaEvent::Subscribe);
}

void Kpf::EventManagerImpl::setupObjectEvents(QSharedPointer<ObjectImpl>& object)
{
    QMutexLocker locker(kpfMutex());
    for (QDomElement config = object->config.firstChildElement(TAG_PUBEVENT);
         !config.isNull();
         config = config.nextSiblingElement(TAG_PUBEVENT))
    {
        QString eventName = config.attribute(KEY_EVENTID);
        QString signal = normalizedSignature(config.attribute(KEY_SIGNAL));
        QString topic = config.attribute(KEY_TOPIC);
        if ((eventName.isEmpty() && signal.isEmpty()) || topic.isEmpty()) {
            continue;
        }

        if (!eventName.isEmpty())
        {
            addPubEvent(object->name, eventName, topic, false);
        }
        else // !signal.isEmpty()
        {
            addPubEvent(object->name, signal, topic, true);
        }
    }

    for (QDomElement config = object->config.firstChildElement(TAG_SUBEVENT);
         !config.isNull();
         config = config.nextSiblingElement(TAG_SUBEVENT))
    {
        QString eventId = config.attribute(KEY_EVENTID);
        QString signal = normalizedSignature(config.attribute(KEY_SIGNAL));
        QString slot = normalizedSignature(config.attribute(KEY_SLOT));
        QString topic = config.attribute(KEY_TOPIC);
        if ((eventId.isEmpty() && slot.isEmpty() && signal.isEmpty())
            || topic.isEmpty()) {
            continue;
        }

        if (!eventId.isEmpty())
        {
            addSubEvent(object->name, eventId, topic, false);
        }
        else if(!slot.isEmpty())
        {
            addSubEvent(object->name, slot, topic, true);
        }
        else // !signal.isEmpty()
        {
            addSubEvent(object->name, signal, topic, true);
        }
    }
}

void Kpf::EventManagerImpl::registerPubSignalEvent(QSharedPointer<ObjectImpl>& object, const QString& eventName)
{
    QMutexLocker locker(kpfMutex());

    SignalSpy* signalSpy = new SignalSpy(object, eventName.toUtf8());
    connect(object->object.data(), &QObject::destroyed,
            [signalSpy]{ delete signalSpy; });

    registerEvent(eventName, &(signalSpy->eventObject), MetaEvent::Publish, object);
}

void Kpf::EventManagerImpl::registerSubSignalSlotEvent(QSharedPointer<Kpf::ObjectImpl>& object, const QString& eventName)
{
    QMutexLocker locker(kpfMutex());

    const QMetaObject* metaObject = object->object->metaObject();
    int index = metaObject->indexOfMethod(eventName.toUtf8().constData());
    if (index < 0) {
        return;
    }
    QMetaMethod method = metaObject->method(index);

    ObjectEvent* eventObject = new ObjectEvent;
    InvokeMethodSyncHelper* invokeHelper;
    invokeHelper = new InvokeMethodSyncHelper(object->object, method);
    eventObject->func = [invokeHelper](const QVariantList& args)->QVariant{
        return invokeHelper->invoke(args);
    };

    connect(object->object.data(), &QObject::destroyed,
            [eventObject, invokeHelper]{
        delete invokeHelper;
        delete eventObject;
    });

    registerEvent(eventName, eventObject, MetaEvent::Subscribe, object);
}

void Kpf::EventManagerImpl::registerEvent(const QString& eventName, ObjectEvent* eventObject, MetaEvent::Type type, QSharedPointer<ObjectImpl> object)
{
    QMutexLocker locker(kpfMutex());

    QSharedPointer<ObjectImpl> currentObj;
    if (object) {
        currentObj = object;
    } else {
        currentObj = kpfObjectImpl.currentObject().toStrongRef();
    }
    if (!currentObj) {
        return;
    }
    eventObject->object = currentObj;

    QSharedPointer<MetaClassImpl> classImpl = currentObj->objectClass
                                          .staticCast<MetaClassImpl>();
    if (!classImpl) {
        return;
    }

    QSharedPointer<MetaEventImpl> event;
    if (type == MetaEvent::Publish) {
        event = classImpl->publishedEvents.value(eventName);
    } else {
        event = classImpl->publishedEvents.value(eventName);
    }

    if (!event)
    {
        event = MetaEventImpl::create(MetaEvent::Publish,
                                  eventName,
                                  classImpl->name);
        if (type == MetaEvent::Publish) {
            classImpl->publishedEvents.insert(eventName, event);
        } else {
            classImpl->subscribedEvents.insert(eventName, event);
        }
    }
    event->eventObjects.insert(eventObject);

    eventObject->event = event;

    if (type == MetaEvent::Publish)
    {
        currentObj->publishedEvents.insert(eventName, eventObject);
        notify(&N::pubEventRegistered, eventName);
    }
    else
    {
        currentObj->subscribedEvents.insert(eventName, eventObject);
        notify(&N::subEventRegistered, eventName);
    }
}
