#pragma once
#include <Kpf/Common.h>

// ======== API声明 ========
namespace Kpf {
// 变参模板函数，将任意多个输入值打包为QVariantList
template<typename... Args>
QVariantList packParams(Args&&... args);

/**
 * @brief InvokeMethodSyncHelper类，用于在目标对象线程中执行回调函数，等待并获取返回值
 */
class InvokeMethodSyncHelperPrivate;
class KPFSHARED_EXPORT InvokeMethodSyncHelper : public QObject
{
    Q_OBJECT
public:
    // function为反射执行的函数，该函数会在context所在线程中执行
    InvokeMethodSyncHelper(QObject* context, const std::function<void(void)>& function);
    // function为反射执行的函数，该函数会在context所在线程中执行
    InvokeMethodSyncHelper(QObject* context, const std::function<QVariant(void)>& function);
    // method为object对象中的的函数，该函数会在object所在线程中执行，arguments为参数列表
    InvokeMethodSyncHelper(QObject* object, const QMetaMethod& method);

    // 在目标对象所属线程中执行构造函数中的回调函数，阻塞等待函数执行完毕，返回执行结果
    QVariant invoke(bool* ok = nullptr);
    QVariant invoke(const QVariantList& args, bool* ok = nullptr);

private:
    Q_DECLARE_PRIVATE(InvokeMethodSyncHelper)
    InvokeMethodSyncHelperPrivate* d_ptr;
};
} // namespace Kpf
// ======== API声明 ========

namespace Kpf {
inline void packParamsHelper(QVariantList& ret)
{
    Q_UNUSED(ret);
}
template<typename Arg>
void packParamsHelper(QVariantList& ret, Arg&& arg)
{
    ret << QVariant::fromValue(arg);
}
// QVariant::fromValue compile failes with [ Arg = char* ]
inline void packParamsHelper(QVariantList& ret, char* arg)
{
    ret << QVariant::fromValue(QString::fromUtf8(arg));
}
inline void packParamsHelper(QVariantList& ret, const char* arg)
{
    ret << QVariant::fromValue(QString::fromUtf8(arg));
}

template<typename Arg, typename... Args>
void packParamsHelper(QVariantList& ret, Arg&& arg, Args&&... args)
{
    ret << QVariant::fromValue(arg);
    packParamsHelper(ret, std::forward<Args>(args)...);
}
// QVariant::fromValue compile failes with [ Arg = char* ]
template<typename... Args>
void packParamsHelper(QVariantList& ret, char* arg, Args&&... args)
{
    ret << QVariant::fromValue(QString::fromUtf8(arg));
    packParamsHelper(ret, std::forward<Args>(args)...);
}
template<typename... Args>
void packParamsHelper(QVariantList& ret, const char* arg, Args&&... args)
{
    ret << QVariant::fromValue(QString::fromUtf8(arg));
    packParamsHelper(ret, std::forward<Args>(args)...);
}

template<typename... Args>
QVariantList packParams(Args&&... args)
{
    QVariantList ret;
    packParamsHelper(ret, std::forward<Args>(args)...);
    return ret;
}
} // namespace Kpf
