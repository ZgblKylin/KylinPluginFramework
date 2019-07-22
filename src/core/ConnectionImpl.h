#pragma once
#include <Kpf/Kpf.h>
#include <Kpf/Connection.h>
#include "CommonPrivate.h"
#include "ObjectImpl.h"

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

namespace Kpf {
struct ConnectionImpl : public Connection
{
    virtual ~ConnectionImpl() override;

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
    virtual ~ConnectionManagerImpl() override;

    static ConnectionManagerImpl& instance();

    // ConnectionManager interface
    virtual const QWeakPointer<Connection> createConnection(const QString& sender, const QString& signal, const QString& receiver, const QString& slot, Qt::ConnectionType type = Qt::AutoConnection) override;
    virtual const QWeakPointer<Connection> createConnection(const QDomElement& config) override;
    virtual void removeConnection(QWeakPointer<Connection> connection) override;
    virtual QList<QWeakPointer<Connection> > connections(const QString& name) const override;

private:
    QMetaObject::Connection createConnection(const QString& senderName, QObject* senderPtr, QString signalName, const QString& receiverName, QObject* receiverPtr, QString slotName, Qt::ConnectionType type);
};
} // namespace Kpf
#define kpfConnectionImpl Kpf::ConnectionManagerImpl::instance()

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
