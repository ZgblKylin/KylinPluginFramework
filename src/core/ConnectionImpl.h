#ifndef CONNECTIONIMPL_H
#define CONNECTIONIMPL_H

#include <Kpf/Kpf.h>
#include <Kpf/Connection.h>
#include "CommonPrivate.h"
#include "ObjectImpl.h"

namespace Kpf {
struct ConnectionImpl : public Connection
{
    virtual ~ConnectionImpl();

    static QSharedPointer<ConnectionImpl> create(QString sender,
                                                 QByteArray signalName,
                                                 QString receiver,
                                                 QByteArray slotName,
                                                 Qt::ConnectionType type,
                                                 QWeakPointer<ObjectImpl> senderPtr,
                                                 QWeakPointer<ObjectImpl> receiverPtr);

    QMetaObject::Connection connection;
    QWeakPointer<ObjectImpl> senderPtr;
    QWeakPointer<ObjectImpl> receiverPtr;

private:
    ConnectionImpl();
};

class ConnectionManagerImpl : public ConnectionManager, public NotifyManager<IConnectionNotifier>
{
    Q_OBJECT
public:
    ConnectionManagerImpl();
    virtual ~ConnectionManagerImpl();

    static ConnectionManagerImpl& instance();

    // ConnectionManager interface
    virtual const QWeakPointer<Connection> createConnection(const QString& sender, const QString& signal, const QString& receiver, const QString& slot, Qt::ConnectionType type = Qt::AutoConnection) override;
    virtual const QWeakPointer<Connection> createConnection(const QJsonObject& config) override;
    virtual void removeConnection(QWeakPointer<Connection> connection) override;
    virtual QList<QWeakPointer<Connection> > connections(const QString& name) const override;
    virtual void registerNotifier(IConnectionNotifier* notifier) override;
    virtual void unregisterNotifier(IConnectionNotifier* notifier) override;

private:
    QMetaObject::Connection createConnection(const QString& senderName, QObject* senderPtr, QString signalName, const QString& receiverName, QObject* receiverPtr, QString slotName, Qt::ConnectionType type);
};
} // namespace Kpf
#define kpfConnectionImpl Kpf::ConnectionManagerImpl::instance()

#endif // CONNECTIONIMPL_H
