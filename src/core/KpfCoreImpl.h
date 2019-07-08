#ifndef KPFCOREIMPL_H
#define KPFCOREIMPL_H

#include <Kpf/Kpf.h>
#include <Kpf/Debugging/Debugging.h>
#include "CommonPrivate.h"
#include "ConnectionImpl.h"
#include "ClassImpl.h"
#include "ThreadImpl.h"
#include "ObjectImpl.h"
#include "Library.h"
#include "EventImpl.h"
#include "TopicImpl.h"
#include "ConnectionImpl.h"
#include "KpfLogPrivate.h"

namespace Kpf {
class KpfCoreImpl : public KpfCore, public NotifyManager<ICoreNotifier>
{
public:
    struct SignalEvent
    {
        QString className;
        QString eventId;
        QMetaMethod slot;
    };

    virtual ~KpfCoreImpl();

    static KpfCoreImpl& instance();
    ClassManagerImpl& classManager();
    ThreadManagerImpl& threadManager();
    ObjectManagerImpl& objectManager();
    EventManagerImpl& eventManager();
    TopicManagerImpl& topicManager();
    ConnectionManagerImpl& connectionManager();
    QMutex* mutex();

    bool closingDown() const;

    QWeakPointer<Library> currentLibrary() const;
    void removeLibrary(QSharedPointer<Library> library);

    // KpfCore interface
    virtual bool init(int argc, char *argv[]) override;
    virtual void registerNotifier(ICoreNotifier* notifier) override;
    virtual void unregisterNotifier(ICoreNotifier* notifier) override;

private:
    KpfCoreImpl();

    bool loadPlugins();
    bool loadAppConfig();
    bool loadComponent(const QDir& dir);
    template<typename Value>
    void expandComponents(Value& value);
    bool initConnections();
    bool processInitializations();

    void expandObjectComponent(QJsonObject& element);
    QJsonArray expandStringComponent(QJsonValueRef& ref);

    void atExit();

    QMutex mtx;
    QJsonObject rootNode;
    QJsonArray objectsNode;
    QJsonArray connectionsNode;
    QJsonArray initializationsNode;
    QMap<QString, QJsonValue> componentsNode;

    QScopedPointer<ClassManagerImpl> classManagerImpl;
    QScopedPointer<ThreadManagerImpl> threadManagerImpl;
    QScopedPointer<ObjectManagerImpl> objectManagerImpl;
    QScopedPointer<EventManagerImpl> eventManagerImpl;
    QScopedPointer<TopicManagerImpl> topicManagerImpl;
    QScopedPointer<ConnectionManagerImpl> connectionManagerImpl;

    bool isClosingDown;
    QWeakPointer<Library> currentLib;
    QList<QSharedPointer<Library>> libraries;
};
} // namespace Kpf
#define kpfCoreImpl Kpf::KpfCoreImpl::instance()

KPF_REGISTER(QPauseAnimation)
KPF_REGISTER(QPropertyAnimation)
KPF_REGISTER(QVariantAnimation)
KPF_REGISTER(QFile)
KPF_REGISTER(QFileSelector)
KPF_REGISTER(QFileSystemWatcher)
KPF_REGISTER(QProcess)
KPF_REGISTER(QSaveFile)
KPF_REGISTER(QIdentityProxyModel)
KPF_REGISTER(QSortFilterProxyModel)
KPF_REGISTER(QStringListModel)
KPF_REGISTER(QEventLoop)
KPF_REGISTER(QObject)
KPF_REGISTER(QSharedMemory)
KPF_REGISTER(QTimer)
KPF_REGISTER(QTranslator)
KPF_REGISTER(QLibrary)
KPF_REGISTER(QPluginLoader)
KPF_REGISTER(QEventTransition)
KPF_REGISTER(QFinalState)
KPF_REGISTER(QHistoryState)
KPF_REGISTER(QSignalTransition)
KPF_REGISTER(QState)
KPF_REGISTER(QStateMachine)
KPF_REGISTER(QThread)
KPF_REGISTER(QThreadPool)
KPF_REGISTER(QTimeLine)

KPF_REGISTER(Debugging)

#endif // KPFCOREIMPL_H
