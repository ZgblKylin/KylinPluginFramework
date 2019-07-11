#include "ClassImpl.h"
#include "KpfPrivate.h"

Kpf::MetaClass::~MetaClass()
{
}

Kpf::MetaClass::MetaClass()
{
}

Kpf::MetaClassImpl::~MetaClassImpl()
{
}

QSharedPointer<Kpf::MetaClassImpl> Kpf::MetaClassImpl::create(QSharedPointer<Library> library, const QString& name, QMetaObject metaObject, const std::function<void(Object*)>& constructor)
{
    QSharedPointer<MetaClassImpl> ret(new MetaClassImpl);
    ret->library = library;
    ret->name = name;
    ret->metaObject = metaObject;
    ret->constructor = constructor;
    return ret;
}

Kpf::MetaClassImpl::MetaClassImpl()
{
}

Kpf::ClassManager& Kpf::ClassManager::instance()
{
    return kpfCoreImpl.classManager();
}

Kpf::ClassManagerImpl::ClassManagerImpl()
{
}

Kpf::ClassManagerImpl::~ClassManagerImpl()
{
    QMutexLocker locker(kpfMutex());
    while (!classNames.isEmpty()) {
        classes.remove(classNames.pop());
    }
}

Kpf::ClassManagerImpl& Kpf::ClassManagerImpl::instance()
{
    return kpfCoreImpl.classManager();
}

QStringList Kpf::ClassManagerImpl::availableClassNames() const
{
    QMutexLocker locker(kpfMutex());
    return classes.keys();
}

QWeakPointer<Kpf::MetaClass> Kpf::ClassManagerImpl::findClass(const QString& className) const
{
    QMutexLocker locker(kpfMutex());
    return classes.value(className);
}

void Kpf::ClassManagerImpl::registerClass(const QString& className, QMetaObject metaObject, const std::function<void(Object*)>& constructor)
{
    QMutexLocker locker(kpfMutex());
    QSharedPointer<MetaClassImpl> classImpl = MetaClassImpl::create(kpfCoreImpl.currentLibrary().toStrongRef(),
                                                            className,
                                                            metaObject,
                                                            constructor);
    classNames.push(className);
    classes.insert(className, classImpl);
    notify(&N::classRegistered, className);
}
