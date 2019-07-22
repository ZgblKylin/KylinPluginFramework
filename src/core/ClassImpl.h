#pragma once
#include <Kpf/Kpf.h>
#include <Kpf/Class.h>
#include "CommonPrivate.h"
#include "Library.h"
#include "EventImpl.h"

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

namespace Kpf {
struct MetaClassImpl : public MetaClass
{
    virtual ~MetaClassImpl() override;

    static QSharedPointer<MetaClassImpl> create(QSharedPointer<Library> library,
                                            const QString& name,
                                            QMetaObject metaObject,
                                            const std::function<void(Object*)>& constructor);

    QSharedPointer<Library> library;

    std::function<void(Object*)> constructor;

    QMap<QString, QSharedPointer<MetaEventImpl>> publishedEvents;
    QMap<QString, QSharedPointer<MetaEventImpl>> subscribedEvents;

private:
    MetaClassImpl();
};

class ClassManagerImpl : public ClassManager, public NotifyManager<IClassNotifier>
{
    Q_OBJECT
public:
    ClassManagerImpl();
    virtual ~ClassManagerImpl() override;

    static ClassManagerImpl& instance();

    // ClassManager interface
    virtual QStringList availableClassNames() const override;
    virtual QWeakPointer<MetaClass> findClass(const QString& className) const override;

protected:
    virtual void registerClass(const QString& className, QMetaObject metaObject, const std::function<void(Object*)>& constructor) override;

    QStack<QString> classNames;
    QMap<QString, QSharedPointer<MetaClassImpl>> classes;
};
} // namespace Kpf
#define kpfClassImpl Kpf::ClassManagerImpl::instance()

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
