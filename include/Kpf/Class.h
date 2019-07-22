#pragma once
#include <Kpf/Common.h>
#include <Kpf/Object.h>

// ======== API声明 ========

// 获取ClassManager对象，等同于Kpf::ClassManager::instance()
#define kpfClass
// 注册类型至框架，Class为类型
#define KPF_REGISTER(Class)
// 注册类型至框架，Class为类型，className为配置文件中实际使用的类型名称
#define KPF_REGISTER_CLASS(Class, className)

namespace Kpf {
struct KPFSHARED_EXPORT MetaClass
{
    virtual ~MetaClass();

    QString name;
    QMetaObject metaObject;

protected:
    MetaClass();
};

struct IClassNotifier
{
    virtual ~IClassNotifier() = default;

    virtual void classRegistered(const QString& className) { Q_UNUSED(className) }
};

class KPFSHARED_EXPORT ClassManager : public QObject, virtual public INotifyManager<IClassNotifier>
{
    Q_OBJECT

public:
    virtual ~ClassManager() = default;

    static ClassManager& instance();

    // 注册类型至框架，模板类型T为需要注册的类型，className为配置文件中使用的类名
    // 类型必须继承自QObject，可以继承自Kpf::Base
    template<typename T> void registerClass(const QString& className);
    // 注册类型至框架，模板类型T为需要注册的类型，className默认与T类名相同
    // 类型必须继承自QObject，可以继承自Kpf::Base
    template<typename T> void registerClass();

    // 获取当前可用的类名列表
    virtual QStringList availableClassNames() const = 0;
    // 提供类名，寻找对应的类
    virtual QWeakPointer<MetaClass> findClass(const QString& className) const = 0;

protected:
    // 注册类型至框架，className为配置文件中使用的类名，constructor为返回类对象的工厂函数
    virtual void registerClass(const QString& className, QMetaObject metaObject, const std::function<void(Object*)>& constructor) = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfClass
#define kpfClass Kpf::ClassManager::instance()

#undef KPF_REGISTER_CLASS
#define KPF_REGISTER_CLASS(Class, className) \
struct KpfRegisterHelper##className \
{ \
    KpfRegisterHelper##className() \
    { \
        kpfClass.registerClass<Class>(#className); \
    } \
}; \
static KpfRegisterHelper##className kpfRegisterHelper##className;

#undef KPF_REGISTER
#define KPF_REGISTER(Class) KPF_REGISTER_CLASS(Class, Class)


namespace Kpf {
template<typename T>
inline void ClassManager::registerClass(const QString& className)
{
    auto constructor = [](Object* object){
        T* ptr = new(std::nothrow) T;
        object->object = ptr;
        object->base = toBase<Base>(ptr);
    };
    registerClass(className, T::staticMetaObject, constructor);
}

template<typename T>
inline void ClassManager::registerClass()
{
    registerClass<T>(T::staticMetaObject.className());
}
} // namesapce Kpf
