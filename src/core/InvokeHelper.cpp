#include "InvokeHelperPrivate.h"
#include "KpfPrivate.h"
#include <typeinfo>

Kpf::InvokeMethodSyncHelper::InvokeMethodSyncHelper(QObject* context, const std::function<void(void)>& function)
    : d_ptr(new InvokeMethodSyncHelperPrivate(this, context, function))
{
}

Kpf::InvokeMethodSyncHelper::InvokeMethodSyncHelper(QObject* context, const std::function<QVariant(void)>& function)
    : d_ptr(new InvokeMethodSyncHelperPrivate(this, context, function))
{
}

Kpf::InvokeMethodSyncHelper::InvokeMethodSyncHelper(QObject* object, const QMetaMethod& method)
    : d_ptr(new InvokeMethodSyncHelperPrivate(this, object, method))
{
}

QVariant Kpf::InvokeMethodSyncHelper::invoke(bool* ok)
{
    Q_D(InvokeMethodSyncHelper);
    return d->invoke(QVariantList(), ok);
}

QVariant Kpf::InvokeMethodSyncHelper::invoke(const QVariantList& args, bool* ok)
{
    Q_D(InvokeMethodSyncHelper);
    return d->invoke(args, ok);
}

Kpf::InvokeMethodSyncHelperPrivate::InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context)
    : QObject(parent), q_ptr(parent), object(context)
{
    Q_Q(InvokeMethodSyncHelper);
    q->moveToThread(object->thread());
}

Kpf::InvokeMethodSyncHelperPrivate::InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context, const std::function<void(void)>& function)
    : InvokeMethodSyncHelperPrivate(parent, context)
{
    if (object->thread() == QThread::currentThread())
    {
        invokeFunction = [this, function](const QVariantList&)
        {
            invoked = true;
            function();
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for function" << function.target_type().name()
                         << "finished";
        };
    }
    else
    {
        connect(this, &InvokeMethodSyncHelperPrivate::invokeSignal,
                this, [function]{ function(); },
                Qt::BlockingQueuedConnection);
        invokeFunction = [this, function](const QVariantList& args)
        {
            invoked = true;
            emit invokeSignal(args);
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for function" << function.target_type().name()
                         << "finished";
        };
    }
}

Kpf::InvokeMethodSyncHelperPrivate::InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* context, const std::function<QVariant(void)>& function)
    : InvokeMethodSyncHelperPrivate(parent, context)
{
    if (object->thread() == QThread::currentThread())
    {
        invokeFunction = [this, function](const QVariantList&)
        {
            ret = function();
            invoked = true;
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for function" << function.target_type().name()
                         << "finished with return value"
                         << ret;
        };
    }
    else
    {
        connect(this, &InvokeMethodSyncHelperPrivate::invokeSignal,
                this, [this, function]{ ret = function(); },
                Qt::BlockingQueuedConnection);
        invokeFunction = [this, function](const QVariantList& args)
        {
            emit invokeSignal(args);
            invoked = true;
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for function" << function.target_type().name()
                         << "finished with return value"
                         << ret;
        };
    }
}

Kpf::InvokeMethodSyncHelperPrivate::InvokeMethodSyncHelperPrivate(InvokeMethodSyncHelper* parent, QObject* o, const QMetaMethod& m)
    : InvokeMethodSyncHelperPrivate(parent, o)
{
    method = m;

    if (object->thread() == QThread::currentThread())
    {
        invokeFunction = [this](const QVariantList& args)
        {
            invokeMethod(args);
        };
    }
    else
    {
        connect(this, &InvokeMethodSyncHelperPrivate::invokeSignal,
                this, &InvokeMethodSyncHelperPrivate::invokeMethod,
                Qt::BlockingQueuedConnection);
        invokeFunction = [this](const QVariantList& args)
        {
            emit invokeSignal(args);
        };
    }
}

QVariant Kpf::InvokeMethodSyncHelperPrivate::invoke(const QVariantList& args, bool* ok)
{
    if (!object) {
        return false;
    }

    invokeFunction(args);

    if (ok) {
        *ok = invoked;
    }

    return ret;
}

void Kpf::InvokeMethodSyncHelperPrivate::invokeMethod(QVariantList args)
{
    QGenericArgument arguments[10];
    for (int i = 0; i < method.parameterCount(); ++i)
    {
        args[i].convert(method.parameterType(i));
        arguments[i] = QGenericArgument(args[i].typeName(), args[i].data());
    }

    if (method.returnType() == QMetaType::Void)
    {
        invoked = method.invoke(object,
                                Qt::DirectConnection,
                                arguments[0],
                                arguments[1],
                                arguments[2],
                                arguments[3],
                                arguments[4],
                                arguments[5],
                                arguments[6],
                                arguments[7],
                                arguments[8],
                                arguments[9]);
        if (invoked)
        {
            qCDebug(kpf) << "Invoke object" << object->objectName()
                             << "for method" << method.methodSignature()
                             << "with params" << args
                             << "successed";
        }
        else
        {
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for method" << method.methodSignature()
                         << "with params" << args
                         << "failed";
        }
    }
    else
    {
        ret = QVariant(QVariant::Type(method.returnType()));
        QGenericReturnArgument retValue(ret.typeName(), ret.data());
        invoked = method.invoke(object,
                                Qt::DirectConnection,
                                retValue,
                                arguments[0],
                                arguments[1],
                                arguments[2],
                                arguments[3],
                                arguments[4],
                                arguments[5],
                                arguments[6],
                                arguments[7],
                                arguments[8],
                                arguments[9]);
        if (invoked)
        {
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for method" << method.methodSignature()
                         << "with params" << args
                         << "successed with return value"
                         << QVariant(QMetaType::type(retValue.name()), retValue.data());
        }
        else
        {
            qCDebug(kpf) << "Invoke object" << object->objectName()
                         << "for method" << method.methodSignature()
                         << "with params" << args
                         << "failed";
        }
    }
}
