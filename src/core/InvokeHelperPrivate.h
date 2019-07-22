#pragma once
#include <Kpf/Kpf.h>

namespace Kpf {
struct ObjectImpl;
class InvokeMethodSyncHelperPrivate : public QObject
{
    Q_OBJECT

public:
    InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context);
    InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context, const std::function<void(void)>& function);
    InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context, const std::function<QVariant(void)>& function);
    InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* object, const QMetaMethod& method);

    QVariant invoke(const QVariantList& args, bool* ok);

private:
    Q_SIGNAL void invokeSignal(const QVariantList& args);
    void invokeMethod(QVariantList args);

    Q_DECLARE_PUBLIC(InvokeMethodSyncHelper)
    InvokeMethodSyncHelper* q_ptr;

    QObject* object = nullptr;
    QMetaMethod method;

    bool invoked = false;
    QVariant ret;

    std::function<void(const QVariantList&)> invokeFunction;
};
} //namespace Kpf
