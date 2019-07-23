#include "ConnectionImpl.h"
#include "KpfPrivate.h"

Kpf::ConnectionImpl::~ConnectionImpl()
{
}

QSharedPointer<Kpf::ConnectionImpl> Kpf::ConnectionImpl::create(QString sender, QByteArray signalName, QString receiver, QByteArray slotName, Qt::ConnectionType type, QWeakPointer<ObjectImpl> senderPtr, QWeakPointer<ObjectImpl> receiverPtr)
{
    QSharedPointer<ConnectionImpl> ret(new ConnectionImpl);
    ret->sender = sender;
    ret->signalName = signalName;
    ret->receiver = receiver;
    ret->slotName = slotName;
    ret->type = type;
    ret->senderPtr = senderPtr;
    ret->receiverPtr = receiverPtr;
    return ret;
}

Kpf::ConnectionImpl::ConnectionImpl()
{
}

Kpf::ConnectionManagerImpl::ConnectionManagerImpl()
{
}

Kpf::ConnectionManagerImpl::~ConnectionManagerImpl()
{
}

Kpf::ConnectionManagerImpl& Kpf::ConnectionManagerImpl::instance()
{
    return kpfCoreImpl.connectionManager();
}

const QWeakPointer<Kpf::Connection> Kpf::ConnectionManagerImpl::createConnection(const QString& senderName, const QString& signalName, const QString& receiverName, const QString& slotName, Qt::ConnectionType type)
{
    QMutexLocker locker(kpfMutex());
    QSharedPointer<ObjectImpl> sender = kpfObject.findObject(senderName)
                                        .toStrongRef()
                                        .staticCast<ObjectImpl>();
    if (!sender)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": cannot find sender"<< senderName;
        return {};
    }

    QSharedPointer<ObjectImpl> receiver = kpfObject.findObject(receiverName)
                                          .toStrongRef()
                                          .staticCast<ObjectImpl>();
    if(!receiver)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": cannot find receiver"<< receiverName;
        return {};
    }

    QObject* senderPtr = sender->object;
    QObject* receiverPtr = receiver->object;

    QMetaObject::Connection connection;
    QSharedPointer<ConnectionImpl> c = ConnectionImpl::create(senderName,
                                                              signalName.toUtf8(),
                                                              receiverName,
                                                              slotName.toUtf8(),
                                                              type,
                                                              sender,
                                                              receiver);
    c->sender = senderName;
    c->signalName = signalName.toUtf8();
    c->receiver = receiverName;
    c->slotName = slotName.toUtf8();
    c->type = type;
    c->senderPtr = sender;
    c->receiverPtr = receiver;

    connection = createConnection(senderName, senderPtr, signalName,
                                  receiverName, receiverPtr, slotName,
                                  type);
    c->connection = connection;
    if (connection)
    {
        sender->connections.append(c);
        receiver->connections.append(c);
        return c.staticCast<Connection>();
    }
    else
    {
        return {};
    }
}

const QWeakPointer<Kpf::Connection> Kpf::ConnectionManagerImpl::createConnection(const QDomElement& config)
{
    QMutexLocker locker(kpfMutex());

    QString sender = config.attribute(KEY_SENDER);
    QString signal = config.attribute(KEY_SIGNAL);
    QString receiver = config.attribute(KEY_RECEIVER);
    QString slot = config.attribute(KEY_SLOT);
    QString type = config.attribute(KEY_TYPE, ConnectionTypes.key(Qt::AutoConnection));
    if(!ConnectionTypes.contains(type)) {
        qCWarning(kpf) << "Invalid connection type" << type;
    }

    return createConnection(sender, signal, receiver, slot, ConnectionTypes.value(type));
}

void Kpf::ConnectionManagerImpl::removeConnection(QWeakPointer<Connection> connection)
{
    QMutexLocker locker(kpfMutex());

    QSharedPointer<ConnectionImpl> c = connection.toStrongRef().staticCast<ConnectionImpl>();

    auto compareFunc = [c](const QSharedPointer<ConnectionImpl>& obj)->bool
    { return obj == c; };

    QSharedPointer<ObjectImpl> sender = kpfObject.findObject(c->sender)
                                        .toStrongRef()
                                        .staticCast<ObjectImpl>();
    auto it = std::find_if(sender->connections.begin(),
                           sender->connections.end(),
                           compareFunc);
    while (it != sender->connections.end())
    {
        sender->connections.erase(it);
        it = std::find_if(sender->connections.begin(),
                          sender->connections.end(),
                          compareFunc);
    }

    QSharedPointer<ObjectImpl> receiver = kpfObject.findObject(c->receiver)
                                          .toStrongRef()
                                          .staticCast<ObjectImpl>();
    it = std::find_if(receiver->connections.begin(),
                      receiver->connections.end(),
                      compareFunc);
    while (it != receiver->connections.end())
    {
        receiver->connections.erase(it);
        it = std::find_if(receiver->connections.begin(),
                          receiver->connections.end(),
                          compareFunc);
    }

    if (!sender && !receiver) {
        return;
    }

    disconnect(c->connection);

    qCDebug(kpf) << "Remove connection"
                 << "from sender" << c->sender
                 << "of signal" << c->signalName
                 << "to receiver" << c->receiver
                 << "of method" << c->slotName
                 << "with type" << c->type;
    notify(&N::connectionRemoved, c->sender, c->signalName, c->receiver, c->slotName, c->type);
}

QList<QWeakPointer<Kpf::Connection>> Kpf::ConnectionManagerImpl::connections(const QString& name) const
{
    QMutexLocker locker(kpfMutex());

    QList<QWeakPointer<Connection>> ret;
    QSharedPointer<ObjectImpl> object = kpfObject.findObject(name)
                                        .toStrongRef()
                                        .staticCast<ObjectImpl>();
    if (!object) {
        return ret;
    }
    for (QSharedPointer<ConnectionImpl> connection : object->connections)
    {
        ret.append(connection.staticCast<Connection>());
    }
    return ret;
}

QMetaObject::Connection Kpf::ConnectionManagerImpl::createConnection(const QString& senderName, QObject* senderPtr, QString signalName, const QString& receiverName, QObject* receiverPtr, QString slotName, Qt::ConnectionType type)
{
    QMutexLocker locker(kpfMutex());

    if (!senderPtr)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": sender is nullptr";
        return {};
    }
    if(!receiverPtr)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": receiver is nullptr";
        return {};
    }

    const QMetaObject* senderClass = senderPtr->metaObject();
    const QMetaObject* receiverClass = receiverPtr->metaObject();

    signalName = normalizedSignature(signalName);
    slotName = normalizedSignature(slotName);

    QMetaMethod signal;
    bool find = false;
    int index = senderClass->indexOfMethod(signalName.toUtf8().constData());
    if (index >= 0)
    {
        signal = senderClass->method(index);
        find = (signal.methodType() == QMetaMethod::Signal);
    }
    if (!find)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": cannot find given signal in sender";
        return {};
    }

    QMetaMethod slot;
    find = false;
    index = receiverClass->indexOfMethod(slotName.toUtf8().constData());
    if (index >= 0)
    {
        slot = receiverClass->method(index);
        find = ((slot.methodType() != QMetaMethod::Constructor)
                && (slot.methodType() != QMetaMethod::Method));
    }
    if (!find)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": cannot find given method in receiver";
        return {};
    }

    QMetaObject::Connection connection = connect(senderPtr,
                                                 signal,
                                                 receiverPtr,
                                                 slot,
                                                 type);
    if (!connection)
    {
        qCWarning(kpf) << "Create connection failed"
                       << "from sender" << senderName
                       << "of signal" << signalName
                       << "to receiver" << receiverName
                       << "of method" << slotName
                       << "with type" << type
                       << ": signal and method are not compatible";
    }

    notify(&N::connectionCreated, senderName, signalName.toUtf8(), receiverName, slotName.toUtf8(), type);

    return connection;
}

Kpf::Connection::~Connection()
{
}

Kpf::ConnectionManager& Kpf::ConnectionManager::instance()
{
    return kpfCoreImpl.connectionManager();
}
