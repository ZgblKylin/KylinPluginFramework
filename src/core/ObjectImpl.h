#pragma once
#include <Kpf/Kpf.h>
#include "CommonPrivate.h"
#include "Library.h"
#include "TopicImpl.h"
#include "Library.h"

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

namespace Kpf {
struct ConnectionImpl;

struct ObjectImpl : public Object
{
    virtual ~ObjectImpl() override;

    static QSharedPointer<ObjectImpl> create(QSharedPointer<MetaClass> objectClass,
                                             const QString& name,
                                             const QDomElement& config);

    QSharedPointer<Library> library;

    QList<QSharedPointer<ConnectionImpl>> connections;

    QMap<QString, ObjectEvent*> publishedEvents;
    QMap<QString, ObjectEvent*> subscribedEvents;

private:
    ObjectImpl();
};

class ObjectManagerImpl : public ObjectManager, public NotifyManager<IObjectNotifier>
{
    Q_OBJECT
public:
    ObjectManagerImpl();
    virtual ~ObjectManagerImpl() override;

    static ObjectManagerImpl& instance();

    // ObjectManager interface
    virtual QStringList objectNames() const override;
    virtual QWeakPointer<Object> findObject(const QString& name) const override;
    virtual QWeakPointer<Object> createObject(QString name, QString className, QDomElement config = QDomElement(), QObject* parent = qApp) override;
    virtual void destroyObject(const QString& name) override;

    QWeakPointer<ObjectImpl> currentObject();

    QWeakPointer<Kpf::ObjectImpl> createObject(QString name, QString className, const QDomElement& objectConfig, Ref<Ptr<QObject>> oParent = defaultObjectParent, Ref<Ptr<QWidget>> wParent = defaultWidgetParent);
    QWeakPointer<Kpf::ObjectImpl> createObject(const QDomElement& objectConfig, Ref<Ptr<QObject>> oParent = defaultObjectParent, Ref<Ptr<QWidget>> wParent = defaultWidgetParent);
    bool createChildren(const QDomElement& config, QObject* oParent = defaultObjectParent, QWidget* wParent = defaultWidgetParent);
    bool createChildren_withoutObject(const QDomElement& config, QObject* oParent = defaultObjectParent, QWidget* wParent = defaultWidgetParent);

    void setObjectParent(QObject* object, const QString& parent, QObject* oParent, QWidget* wParent);
    bool setObjectProperty(QSharedPointer<Kpf::ObjectImpl>& object);
    bool initObject(QSharedPointer<Kpf::ObjectImpl>& object);

private:
    QStringList names;
    QMap<QString, QSharedPointer<ObjectImpl>> objects;
    QWeakPointer<ObjectImpl> currentObj;

    static QObject* defaultObjectParent;
    static QWidget* defaultWidgetParent;
};
} // namespace Kpf
#define kpfObjectImpl Kpf::ObjectManagerImpl::instance()

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
