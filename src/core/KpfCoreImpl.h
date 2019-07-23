#pragma once
#include <Kpf/Kpf.h>
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

#ifdef Q_CC_MSVC
#pragma warning(push)
#pragma warning(disable:4250)
#endif

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

    virtual ~KpfCoreImpl() override;

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

private:
    KpfCoreImpl();

    bool loadPlugins();
    bool loadAppConfig(const QString& appFile);
    bool loadComponents(const QDir& dir);
    void expandComponents();
    void expandComponent(QDomElement& node, QMap<QString, QDomElement>& map);
    bool initConnections();
    bool processInitializations();

    void atExit();

    QMutex mtx;

    QDomDocument appConfig;
    QDomElement rootNode;
    QDomElement objectsNode;
    QDomElement connectionsNode;
    QDomElement initializationsNode;
    QMap<QString, QDomDocument> componentsConfig;
    QMap<QString, QDomElement> objectsComponentsNode;
    QMap<QString, QDomElement> connectionsComponentsNode;
    QMap<QString, QDomElement> initializationsComponentsNode;

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

#ifdef Q_CC_MSVC
#pragma warning(pop)
#endif
