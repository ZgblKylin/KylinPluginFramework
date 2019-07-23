#include "SignalSpy.h"
#include "KpfPrivate.h"

Kpf::SignalSpy::SignalSpy(QSharedPointer<Kpf::ObjectImpl>& object, const QByteArray& eventName)
    : signal(convertSignalName(eventName))
{
    qCDebug(kpf) << "Create signal spy for object" << object->name
                 << ", observing signal event" << eventName;

    const int memberOffset = QObject::staticMetaObject.methodCount();

    const QMetaObject* metaObject = object->object->metaObject();
    const int signalIndex = metaObject->indexOfMethod(eventName.constData());
    if (signalIndex < 0)
    {
        qCWarning(kpf) << "Create signal spy for object" << object->name
                       << "failed: no such signal" << eventName;
        return;
    }

    if (!QMetaObject::connect(object->object,
                              signalIndex,
                              this,
                              memberOffset,
                              Qt::AutoConnection,
                              nullptr))
    {
        qCWarning(kpf) << "Create signal spy for object" << object->name
                       << "failed: QMetaObject::connect returned false";
        return;
    }

    QMetaMethod signalMethod = metaObject->method(signalIndex);
    for (int i = 0; i < signalMethod.parameterCount(); ++i)
    {
        int type = signalMethod.parameterType(i);
        args << QVariant(QVariant::Type(type));
    }
}

int Kpf::SignalSpy::qt_metacall(QMetaObject::Call call, int methodId, void** a)
{
    methodId = QObject::qt_metacall(call, methodId, a);
    if (methodId < 0)
        return methodId;

    if (call != QMetaObject::InvokeMetaMethod) {
        return methodId;
    }

    if (methodId == 0)
    {
        for (int i = 0; i < args.count(); ++i)
        {
            QVariant& arg = args[i];
            QMetaType::Type type = QMetaType::Type(arg.type());
            if (type == QMetaType::QVariant)
                arg = *reinterpret_cast<QVariant *>(a[i + 1]);
            else
                arg = QVariant(type, a[i + 1]);
        }
    }
    --methodId;

    eventObject.post(args);

    return methodId;
}
