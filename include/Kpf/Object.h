#pragma once
#include <Kpf/Common.h>
#include <Kpf/Constants.h>

// ======== API声明 ========

// 获取ObjectManager对象，等同于Kpf::ObjectManager::instance()
#define kpfObject

namespace Kpf {
struct MetaClass;
struct Thread;
struct Object;

// 当需要使用接口类时，接口类需继承自QObject或Kpf::Base
// Kpf::Base可作为多继承环境下的替代品（因为QObject不允许多继承）
struct KPFSHARED_EXPORT Base
{
    explicit Base(QObject* self);
    virtual ~Base();

    QSharedPointer<Object> getObject() const;
    QObject* getQObject() const;

private:
    QObject* self = nullptr;
};

struct KPFSHARED_EXPORT Object
{
    virtual ~Object();

    QSharedPointer<MetaClass> objectClass;
    QString name;
    QDomElement config;
    QPointer<QObject> object;
    Base* base;
    QSharedPointer<Thread> thread;

    static QSharedPointer<Object> getObject(QObject* object);
};

struct IObjectNotifier
{
    virtual ~IObjectNotifier() = default;

    // Object initialization
    virtual void aboutToCreateObject(const QString& className, const QString& objectName) { Q_UNUSED(className) Q_UNUSED(objectName) }
    virtual void objectCreated(const QString& className, const QString& objectName) { Q_UNUSED(className) Q_UNUSED(objectName) }
    virtual void objectDestroyed(const QString& className, const QString& objectName) { Q_UNUSED(className) Q_UNUSED(objectName) }
};

class KPFSHARED_EXPORT ObjectManager : public QObject, virtual public INotifyManager<IObjectNotifier>
{
    Q_OBJECT
public:
    virtual ~ObjectManager() = default;

    static ObjectManager& instance();

    // 获取当前已创建的所有对象名
    virtual QStringList objectNames() const = 0;

    // 提供对象名，寻找对应的对象
    virtual QWeakPointer<Object> findObject(const QString& name) const = 0;
    template<typename T>
    T* findObject(const QString& name) const;

    /**
     * @brief createObject 构造新对象
     * @param className 对象类名，也可配置于config的Class属性，非空时会覆盖config中配置
     * @param name 对象名称，也可配置于config的Name属性，非空时会覆盖config中配置
     * @param config 对象配置节点
     * @param parent 对象的父对象
     * @return 构造的新对象，若构造失败，返回nullptr
     */
    virtual QWeakPointer<Object> createObject(QString name, QString className, QDomElement config = QDomElement(), QObject* parent = qApp) = 0;
    QWeakPointer<Object> createObject(QString className, const QDomElement& config = QDomElement(), QObject* parent = qApp);
    QWeakPointer<Object> createObject(const QDomElement& config, QObject* parent = qApp);
    template<typename T>
    T* createObject(QString name, QString className, const QDomElement& config = QDomElement(), QObject* parent = qApp);
    template<typename T>
    T* createObject(QString className, const QDomElement& config = QDomElement(), QObject* parent = qApp);
    template<typename T>
    T* createObject(const QDomElement& config, QObject* parent = qApp);

    // 销毁对应名称的对象
    virtual void destroyObject(const QString& name) = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfObject
#define kpfObject Kpf::ObjectManager::instance()

namespace Kpf {
inline QObject* Base::getQObject() const
{
    return const_cast<QObject*>(self);
}

template<typename T>
inline T* ObjectManager::findObject(const QString& name) const
{
    QSharedPointer<Object> object = findObject(name).toStrongRef();
    if (!object) return nullptr;
    T* ptr = toDelivered<T>(object->base);
    if (!ptr) {
        ptr = toDelivered<T>(object->object.data());
    }
    return ptr;
}

inline QWeakPointer<Object> ObjectManager::createObject(QString className, const QDomElement& config, QObject* parent)
{
    return createObject(config.attribute(KEY_NAME), className, config, parent);
}

inline QWeakPointer<Object> ObjectManager::createObject(const QDomElement& config, QObject* parent)
{
    return createObject(config.attribute(KEY_NAME), config.attribute(KEY_CLASS), config, parent);
}

template<typename T>
T* ObjectManager::createObject(QString name, QString className, const QDomElement& config, QObject* parent)
{
    QSharedPointer<Object> object = createObject(name, className, config, parent)
                                    .toStrongRef();
    if (!object)
    {
        return nullptr;
    }
    else
    {
        T* ptr = toDelivered<T>(object->base);
        if (!ptr) {
            ptr = toDelivered<T>(object->object.data());
        }
        if (!ptr) {
            destroyObject(object->name);
        }
        return ptr;
    }
}

template<typename T>
T* ObjectManager::createObject(QString className, const QDomElement& config, QObject* parent)
{
    return createObject<T>(config.attribute(KEY_NAME), className, config, parent);
}

template<typename T>
T* ObjectManager::createObject(const QDomElement& config, QObject* parent)
{
    return createObject<T>(config.attribute(KEY_NAME), config.attribute(KEY_CLASS), config, parent);
}
} // namespace Kpf
