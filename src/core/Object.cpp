#include "ObjectImpl.h"
#include "KpfPrivate.h"

static const char* PROPERTY_KPFOBJECT = "KpfObject";
QObject* Kpf::ObjectManagerImpl::defaultObjectParent = nullptr;
QWidget* Kpf::ObjectManagerImpl::defaultWidgetParent = nullptr;

Q_DECLARE_METATYPE(QWeakPointer<Kpf::ObjectImpl>)

Kpf::Base::Base(QObject* s)
    : self(s)
{
}

Kpf::Base::~Base()
{
}

QSharedPointer<Kpf::Object> Kpf::Base::getObject() const
{
    if (self == nullptr) {
        return {};
    }
    return Object::getObject(const_cast<QObject*>(self));
}

Kpf::Object::~Object()
{
}

QSharedPointer<Kpf::Object> Kpf::Object::getObject(QObject* objectPtr)
{
    QWeakPointer<ObjectImpl> object;
    object = objectPtr->property(PROPERTY_KPFOBJECT)
             .value<QWeakPointer<Kpf::ObjectImpl>>();
    return object.toStrongRef().staticCast<Object>();
}

Kpf::ObjectImpl::~ObjectImpl()
{
    if (object)
    {
        if (kpfCoreImpl.closingDown()) {
            delete object.data();
        } else {
            object->deleteLater();
        }
    }
    if (thread)
    {
        thread->objects.remove(this);
        if (thread->objects.isEmpty()) {
            kpfThread.removeThread(thread);
        }
    }
    if (library)
    {
        library->objects.remove(name);
        if (library->objects.isEmpty() && kpfCoreImpl.closingDown()) {
            kpfCoreImpl.removeLibrary(library);
        }
    }
    kpfCInformation("Kpf") << "Object" << name << "destroyed";
}

QSharedPointer<Kpf::ObjectImpl> Kpf::ObjectImpl::create(QSharedPointer<MetaClass> objectClass, const QString& name, const QJsonObject& config)
{
    QSharedPointer<ObjectImpl> ret(new ObjectImpl);
    QSharedPointer<MetaClassImpl> classImpl = objectClass.staticCast<MetaClassImpl>();
    ret->library = classImpl->library;
    if (ret->library) {
        ret->library->objects.insert(name, ret);
    }
    ret->objectClass = objectClass;
    ret->name = name;
    ret->config = config;
    return ret;
}

Kpf::ObjectImpl::ObjectImpl()
{
}

Kpf::ObjectManager& Kpf::ObjectManager::instance()
{
    return kpfCoreImpl.objectManager();
}

Kpf::ObjectManagerImpl::ObjectManagerImpl()
{
    qRegisterMetaType<QWeakPointer<Kpf::ObjectImpl>>();
}

Kpf::ObjectManagerImpl::~ObjectManagerImpl()
{
    QMutexLocker locker(kpfMutex());
    while (!names.isEmpty()) {
        destroyObject(names.last());
    }
    if (!kpfCoreImpl.closingDown())
    {
        while (!objectNames().isEmpty()) {
            qApp->processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

Kpf::ObjectManagerImpl& Kpf::ObjectManagerImpl::instance()
{
    return kpfCoreImpl.objectManager();
}

QStringList Kpf::ObjectManagerImpl::objectNames() const
{
    QMutexLocker locker(kpfMutex());
    return names;
}

QWeakPointer<Kpf::Object> Kpf::ObjectManagerImpl::findObject(const QString& name) const
{
    QMutexLocker locker(kpfMutex());
    return objects.value(name);
}

QWeakPointer<Kpf::Object> Kpf::ObjectManagerImpl::createObject(QString name, QString className, QJsonObject config, QObject* parent)
{
    QMutexLocker locker(kpfMutex());

    if (className.isEmpty()) {
        className = config.value(TAG_CLASS).toString();
    }

    if (name.isEmpty()) {
        name = config.value(TAG_NAME).toString();
        if (name.isEmpty()) {
            name = className + "_" + QUuid::createUuid().toString();
        }
    }

    QWidget* wParent = qobject_cast<QWidget*>(parent);
    QWeakPointer<ObjectImpl> object = createObject(name, className, config, parent, wParent);
    if (object) {
        createChildren(config, parent, wParent);
    }

    return object;
}

void Kpf::ObjectManagerImpl::destroyObject(const QString& name)
{
    QMutexLocker locker(kpfMutex());
    QSharedPointer<ObjectImpl> object = objects.take(name);
    names.removeAll(name);
    if (object)
    {
        QString className = object->objectClass->name;
        object.reset();
        notify(&N::objectDestroyed, className, name);
    }
}

QWeakPointer<Kpf::ObjectImpl> Kpf::ObjectManagerImpl::currentObject()
{
    QMutexLocker locker(kpfMutex());
    return currentObj;
}

QWeakPointer<Kpf::ObjectImpl> Kpf::ObjectManagerImpl::createObject(QString name, QString className, const QJsonObject& objectConfig, Ref<Ptr<QObject>> oParent, Ref<Ptr<QWidget>> wParent)
{
    QMutexLocker locker(kpfMutex());
    Defer raii([this]{ currentObj.clear(); });

    if (name.isEmpty()) {
        name = objectConfig.value(TAG_NAME).toString();
        if (name.isEmpty()) {
            name = className + "_" + QUuid::createUuid().toString();
        }
    }

    if (objects.contains(name))
    {
        kpfCWarning("Kpf") << "Object create failed for"
                           << "name" << name
                           << "with type" << className
                           << ": object already exists";
        return {};
    }

    QSharedPointer<MetaClassImpl> classImpl = kpfClassImpl.findClass(className)
                                          .toStrongRef()
                                          .staticCast<MetaClassImpl>();
    if (!classImpl)
    {
        kpfCWarning("Kpf") << "Object create failed for"
                           << "name" << name
                           << "with type" << className
                           << ": type unregistered";
        return {};
    }

    notify(&N::aboutToCreateObject, className, name);
    QSharedPointer<ObjectImpl> object = ObjectImpl::create(classImpl.staticCast<MetaClass>(),
                                                           name,
                                                           objectConfig);
    names.append(name);
    objects.insert(name, object);

    currentObj = object;
    classImpl->constructor(object.data());
    if (!object->object)
    {
        kpfCWarning("Kpf") << "Object create failed for"
                           << "name" << name
                           << "with type" << className
                           << ": construct failed with nullptr returned";
        destroyObject(object->name);
        return {};
    }

    connect(object->object.data(), &QObject::destroyed,
            this, [this, name]{
        if (!kpfCoreImpl.closingDown()) {
            destroyObject(name);
        }
    }, Qt::DirectConnection);
    object->object->setObjectName(name);
    object->object->setProperty(PROPERTY_KPFOBJECT,
                                QVariant::fromValue(object.toWeakRef()));

    const QString parentName = objectConfig.value(TAG_PARENT).toString();
    setObjectParent(object->object, parentName, oParent, wParent);

    QWidget* widgetPtr = qobject_cast<QWidget*>(object->object);
    if (!widgetPtr) {
        oParent = object->object;
    } else {
        oParent = object->object;
        wParent = widgetPtr;
    }

    kpfThreadImpl.setObjectThread(object);

    kpfEventImpl.setupObjectEvents(object);

    if (!setObjectProperty(object))
    {
        kpfCWarning("Kpf") << "Object create failed for"
                           << "name" << name
                           << "with type" << className
                           << ": cannote setup properties";
        destroyObject(object->name);
        return {};
    }
    if (!initObject(object))
    {
        kpfCWarning("Kpf") << "Object create failed for"
                           << "name" << name
                           << "with type" << className
                           << ": init method return false";
        destroyObject(object->name);
        return {};
    }

    kpfCInformation("Kpf") << "Object create successed for"
                           << "name" << name
                           << "with type" << className;

    notify(&N::objectCreated, className, name);
    return object;
}

QWeakPointer<Kpf::ObjectImpl> Kpf::ObjectManagerImpl::createObject(const QJsonObject& objectConfig, Ref<Ptr<QObject>> oParent, Ref<Ptr<QWidget>> wParent)
{
    QMutexLocker locker(kpfMutex());
    return createObject(objectConfig.value(TAG_NAME).toString(),
                        objectConfig.value(TAG_CLASS).toString(),
                        objectConfig,
                        oParent,
                        wParent);
}

bool Kpf::ObjectManagerImpl::createChildren(const QJsonValue& config, QObject* oParent, QWidget* wParent)
{
    QMutexLocker locker(kpfMutex());

    if (config.isObject())
    {
        QJsonObject objConfig = config.toObject();
        for (auto it = objConfig.begin(); it != objConfig.end(); ++it)
        {
            QObject* o = oParent;
            QWidget* w = wParent;
            switch (it.value().type())
            {
            case QJsonValue::Object:
            {
                if (!createObject(it.value().toObject(), o, w)) {
                    continue;
                }
            }
                [[fallthrough]];
            case QJsonValue::Array:
                createChildren(it.value(), o, w);
                break;
            default:
                break;
            }
        }
    }
    else if (config.isArray())
    {
        QJsonArray arrConfig = config.toArray();
        for (auto it = arrConfig.begin(); it != arrConfig.end(); ++it)
        {
            QObject* o = oParent;
            QWidget* w = wParent;
            switch ((*it).type())
            {
            case QJsonValue::Object:
                if (!createObject((*it).toObject(), o, w)) {
                    continue;
                }
                [[fallthrough]];
            case QJsonValue::Array:
                createChildren(*it, o, w);
                break;
            default:
                break;
            }
        }
    }
    return true;
}

void Kpf::ObjectManagerImpl::setObjectParent(QObject* object, const QString& parent, QObject* oParent, QWidget* wParent)
{
    QMutexLocker locker(kpfMutex());

    if (!parent.isEmpty())
    {
        QObject* parentObj = ObjectManager::findObject<QObject>(parent);
        if (parentObj)
        {
            if (object->inherits("QWidget"))
            {
                if (parentObj->inherits("QWidget"))
                {
                    QWidget* oWidget = qobject_cast<QWidget*>(object);
                    QWidget* pWidget = qobject_cast<QWidget*>(parentObj);
                    oWidget->setParent(pWidget);
                    kpfCLog("Kpf", 1) << "Parent of object" << object->objectName()
                                      << "is set to QWidget" << pWidget->objectName();
                    return;
                }
            }
            else
            {
                object->setParent(parentObj);
                kpfCLog("Kpf", 1) << "Parent of object" << object->objectName()
                                  << "is set to QObject" << parentObj->objectName();
                return;
            }
        }
    }

    QWidget* w = qobject_cast<QWidget*>(object);
    if (!w)
    {
        if (oParent)
        {
            object->setParent(oParent);
            kpfCLog("Kpf", 1) << "Parent of object" << object->objectName()
                              << "is set to QObject" << oParent->objectName();
        }
        else if (wParent)
        {
            object->setParent(wParent);
            kpfCLog("Kpf", 1) << "Parent of object" << object->objectName()
                              << "is set to QWidget" << wParent->objectName();
        }
    }
    else if (wParent)
    {
        w->setParent(wParent);
        kpfCLog("Kpf", 1) << "Parent of object" << object->objectName()
                          << "is set to QWidget" << wParent->objectName();
    }
}

bool Kpf::ObjectManagerImpl::setObjectProperty(QSharedPointer<ObjectImpl>& object)
{
    QMutexLocker locker(kpfMutex());

    for (auto it = object->config.constBegin();
         it != object->config.constEnd();
         ++it)
    {
        switch (it.value().type())
        {
        case QJsonValue::Bool:
        case QJsonValue::Double:
        case QJsonValue::String:
            break;
        default:
            continue;
        }
        QObject* objectPtr = object->object;
        QString name = it.key();
        QVariant value = it.value().toVariant();

        kpfCLog("Kpf", 1) << "Setup object" << object->name
                          << "for property" << name
                          << "value" << value;

        InvokeMethodSyncHelper(objectPtr, [objectPtr, name, value]{
            objectPtr->setProperty(name.toUtf8().constData(), value);
        }).invoke();
    }
    return true;
}

bool Kpf::ObjectManagerImpl::initObject(QSharedPointer<ObjectImpl>& object)
{
    QMutexLocker locker(kpfMutex());

    const QMetaObject* metaObject = object->object->metaObject();
    QMetaMethod method;
    bool find = false;
    for (int i = metaObject->methodCount() - 1; i >= 0; --i)
    {
        method = metaObject->method(i);
        if (method.methodSignature() == INIT_METHOD)
        {
            find = true;
            break;
        }
    }
    if (!find) {
        return true;

    }

    bool ok = false;
    bool ret = InvokeMethodSyncHelper(object->object, method)
               .invoke({ object->config }, &ok).toBool();
    if (ok && ret)
    {
        kpfCInformation("Kpf") << "Initialize object" << object->name
                               << "successed";
        return true;
    }
    return false;
}
