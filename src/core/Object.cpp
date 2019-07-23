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
    qCInfo(kpf) << "Object" << name << "destroyed";
}

QSharedPointer<Kpf::ObjectImpl> Kpf::ObjectImpl::create(QSharedPointer<MetaClass> objectClass, const QString& name, const QDomElement& config)
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

QWeakPointer<Kpf::Object> Kpf::ObjectManagerImpl::createObject(QString name, QString className, QDomElement config, QObject* parent)
{
    QMutexLocker locker(kpfMutex());

    if (className.isEmpty()) {
        className = config.attribute(KEY_CLASS);
    }

    if (name.isEmpty())
    {
        name = config.attribute(KEY_NAME);
        if (name.isEmpty())
        {
            const QString alreadyExist = config.attribute(KEY_ALREADYEXIST);
            if (!alreadyExist.isEmpty())
            {
                QWeakPointer<Kpf::Object> object = findObject(alreadyExist);
                if (object) {
                    return object;
                }
            }

            name = className + "_" + QUuid::createUuid().toString();
            config.setAttribute(KEY_ALREADYEXIST,
                                QStringLiteral("name"));
        }
    }

    if (!parent) {
        parent = qApp;
    }

    QWidget* wParent = qobject_cast<QWidget*>(parent);
    QWeakPointer<ObjectImpl> object = createObject(std::move(name), std::move(className), config, parent, wParent);
    if (object) {
        createChildren_withoutObject(config, parent, wParent);
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

QWeakPointer<Kpf::ObjectImpl> Kpf::ObjectManagerImpl::createObject(QString name, QString className, const QDomElement& objectConfig, Ref<Ptr<QObject>> oParent, Ref<Ptr<QWidget>> wParent)
{
    QMutexLocker locker(kpfMutex());
    defer [this]{ currentObj.clear(); };

    const QString alreadyExist = objectConfig.attribute(KEY_ALREADYEXIST);
    if (!alreadyExist.isEmpty())
    {
        QWeakPointer<Kpf::Object> object = findObject(alreadyExist);
        if (object) {
            return object.toStrongRef().staticCast<Kpf::ObjectImpl>();
        }
    }

    if (name.isEmpty())
    {
        qCWarning(kpf) << "Object create failed for"
                       << "type" << className
                       << ": object name is empty";
        return {};
    }

    QSharedPointer<ObjectImpl> object = findObject(name).toStrongRef()
                                        .staticCast<Kpf::ObjectImpl>();
    if (object)
    {
        if (!object->object->property(KEY_ALREADYEXIST.toUtf8().constData())
            .toString().isEmpty())
        {
            return object;
        }
        qCWarning(kpf) << "Object create failed for"
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
        qCWarning(kpf) << "Object create failed for"
                       << "name" << name
                       << "with type" << className
                       << ": type unregistered";
        return {};
    }

    notify(&N::aboutToCreateObject, className, name);
    object = ObjectImpl::create(classImpl.staticCast<MetaClass>(),
                                name, objectConfig);
    names.append(name);
    objects.insert(name, object);

    currentObj = object;
    classImpl->constructor(object.data());
    if (!object->object)
    {
        qCWarning(kpf) << "Object create failed for"
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

    const QString parentName = objectConfig.attribute(KEY_PARENT);
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
        qCWarning(kpf) << "Object create failed for"
                       << "name" << name
                       << "with type" << className
                       << ": cannote setup properties";
        destroyObject(object->name);
        return {};
    }
    if (!initObject(object))
    {
        qCWarning(kpf) << "Object create failed for"
                       << "name" << name
                       << "with type" << className
                       << ": init method return false";
        destroyObject(object->name);
        return {};
    }

    qCInfo(kpf) << "Object create successed for"
                << "name" << name
                << "with type" << className;

    notify(&N::objectCreated, className, name);
    return object;
}

QWeakPointer<Kpf::ObjectImpl> Kpf::ObjectManagerImpl::createObject(const QDomElement& objectConfig, Ref<Ptr<QObject>> oParent, Ref<Ptr<QWidget>> wParent)
{
    QMutexLocker locker(kpfMutex());
    return createObject(objectConfig.attribute(KEY_NAME),
                        objectConfig.attribute(KEY_CLASS),
                        objectConfig,
                        oParent,
                        wParent);
}

bool Kpf::ObjectManagerImpl::createChildren(const QDomElement& config, QObject* oParent, QWidget* wParent)
{
    QMutexLocker locker(kpfMutex());

    if (config.tagName() == TAG_CLASS)
    {
        QObject* o = oParent;
        QWidget* w = wParent;
        createObject(config, o, w);
    }
    return createChildren_withoutObject(config, oParent, wParent);
}

bool Kpf::ObjectManagerImpl::createChildren_withoutObject(const QDomElement& config, QObject* oParent, QWidget* wParent)
{
    QMutexLocker locker(kpfMutex());

    for (QDomElement child = config.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement())
    {
        QObject* o = oParent;
        QWidget* w = wParent;
        createChildren(child, o, w);
    }
    return true;
}

template<typename T, typename U>
void setParent(T* obj, U* parent)
{
    obj->setParent(parent);
    qCDebug(kpf) << "Parent of"
                 << (std::is_base_of<QWidget, T>::value ? "QWidget" : "QObject")
                 << obj->objectName()
                 << "is set to"
                 << (std::is_base_of<QWidget, U>::value ? "QWidget" : "QObject")
                 << parent->objectName();
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
                    ::setParent(oWidget, pWidget);
                    return;
                }
            }
            else
            {
                ::setParent(object, parentObj);
                return;
            }
        }
    }

    QWidget* widget = qobject_cast<QWidget*>(object);
    if (!widget)
    {
        QLayout* layout = qobject_cast<QLayout*>(object);
        if (layout && wParent)
        {
            ::setParent(object, wParent);
            wParent->setLayout(layout);
        }
        else if (oParent)
        {
            ::setParent(object, oParent);
        }
        else if (wParent)
        {
            ::setParent(object, wParent);
        }
    }
    else if (wParent)
    {
        ::setParent(widget, wParent);
    }
}

bool Kpf::ObjectManagerImpl::setObjectProperty(QSharedPointer<ObjectImpl>& object)
{
    QMutexLocker locker(kpfMutex());

    QDomNamedNodeMap attributes = object->config.attributes();
    for (int i = 0; i < attributes.count(); ++i)
    {
        QDomAttr attr = attributes.item(i).toAttr();

        QObject* objectPtr = object->object;
        QString name = attr.name();
        QVariant value = attr.value();

        qCDebug(kpf) << "Setup object" << object->name
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
               .invoke({ QVariant::fromValue(object->config) }, &ok)
               .toBool();
    if (ok && ret) {
        qCInfo(kpf) << "Initialize object" << object->name << "successed";
        return true;
    }
    return false;
}
