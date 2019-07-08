#ifndef OBJECTIMPL_H
#define OBJECTIMPL_H

#include <Kpf/Kpf.h>
#include "CommonPrivate.h"
#include "Library.h"
#include "TopicImpl.h"
#include "Library.h"

namespace Kpf {
struct ConnectionImpl;

struct ObjectImpl : public Object
{
    virtual ~ObjectImpl();

    static QSharedPointer<ObjectImpl> create(QSharedPointer<MetaClass> objectClass,
                                             const QString& name,
                                             const QJsonObject& config);

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
    virtual ~ObjectManagerImpl();

    static ObjectManagerImpl& instance();

    // ObjectManager interface
    virtual QStringList objectNames() const override;
    virtual QWeakPointer<Object> findObject(const QString& name) const override;
    virtual QWeakPointer<Object> createObject(QString name, QString className, QJsonObject config = QJsonObject(), QObject* parent = qApp) override;
    virtual void destroyObject(const QString& name) override;
    virtual void registerNotifier(IObjectNotifier* notifier) override;
    virtual void unregisterNotifier(IObjectNotifier* notifier) override;

    QWeakPointer<ObjectImpl> currentObject();

    QWeakPointer<Kpf::ObjectImpl> createObject(QString name, QString className, const QJsonObject& objectConfig, Ref<Ptr<QObject>> oParent = defaultObjectParent, Ref<Ptr<QWidget>> wParent = defaultWidgetParent);
    QWeakPointer<Kpf::ObjectImpl> createObject(const QJsonObject& objectConfig, Ref<Ptr<QObject>> oParent = defaultObjectParent, Ref<Ptr<QWidget>> wParent = defaultWidgetParent);
    bool createChildren(const QJsonValue& config, QObject* oParent = defaultObjectParent, QWidget* wParent = defaultWidgetParent);

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

#endif // OBJECTIMPL_H
