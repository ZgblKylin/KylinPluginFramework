#include "KpfCoreImpl.h"
#include "KpfPrivate.h"

static const char* PATH_STR = "PATH";

Kpf::KpfCore& Kpf::KpfCore::instance()
{
    return kpfCoreImpl;
}

Kpf::KpfCoreImpl::~KpfCoreImpl()
{
    QMutexLocker locker(kpfMutex());
    atExit();
}

Kpf::KpfCoreImpl& Kpf::KpfCoreImpl::instance()
{
    static KpfCoreImpl core;
    return core;
}

Kpf::ClassManagerImpl& Kpf::KpfCoreImpl::classManager()
{
    return *classManagerImpl;
}

Kpf::ThreadManagerImpl& Kpf::KpfCoreImpl::threadManager()
{
    return *threadManagerImpl;
}

Kpf::ObjectManagerImpl& Kpf::KpfCoreImpl::objectManager()
{
    return *objectManagerImpl;
}

Kpf::EventManagerImpl& Kpf::KpfCoreImpl::eventManager()
{
    return *eventManagerImpl;
}

Kpf::TopicManagerImpl& Kpf::KpfCoreImpl::topicManager()
{
    return *topicManagerImpl;
}

Kpf::ConnectionManagerImpl& Kpf::KpfCoreImpl::connectionManager()
{
    return *connectionManagerImpl;
}

QMutex* Kpf::KpfCoreImpl::mutex()
{
    return &mtx;
}

bool Kpf::KpfCoreImpl::closingDown() const
{
    return isClosingDown;
}

QWeakPointer<Kpf::Library> Kpf::KpfCoreImpl::currentLibrary() const
{
    QMutexLocker locker(kpfMutex());
    return currentLib;
}

void Kpf::KpfCoreImpl::removeLibrary(QSharedPointer<Library> library)
{
    QMutexLocker locker(kpfMutex());
    QFileInfo path = library->fileInfo;
    libraries.removeAll(library);
    notify(&N::libraryUnloaded, path);
}

bool Kpf::KpfCoreImpl::init(int argc, char *argv[])
{
    QMutexLocker locker(kpfMutex());

    // chech the call validity
    static bool inited = false;
    {
        if (!qApp)
        {
            qCWarning(kpf) << "QApplication must exist before call "
                              "KpfCore::init";
            return false;
        }
        if (inited) {
            return true;
        }
    }

    notify(&N::beginInitialization);

    // initialize default thread object
    {
        QSharedPointer<Thread> defaultThread = kpfThread.defaultThread()
                                               .toStrongRef();
        defaultThread->thread = qApp->thread();
        defaultThread->eventBus->moveToThread(defaultThread->thread);
    }

    // initialize quit behavior
    {
        QObject::connect(qApp, &QCoreApplication::aboutToQuit,
                         qApp, [this]{atExit();}, Qt::DirectConnection);
    }

    // add plugins dir to PATH
    {
        QString env;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
        env = qEnvironmentVariable(PATH_STR);
#else
        env = QString::fromLocal8Bit(qgetenv(PATH_STR));
#endif
        if (!env.isEmpty()) {
            env += ';';
        }
        env += QDir::toNativeSeparators(qApp->applicationDirPath()
                                        + QDir::separator()
                                        + DIR_PLUGINS);
        qputenv(PATH_STR, env.toLocal8Bit());
    }

    // initialize error handler
    {
        kpfObjectImpl.createObject(QStringLiteral("CoreDump"),
                                   QStringLiteral("CoreDump"));
    }

    // initialize logger
    {
        initLogger(argc, argv);
        notify(&N::loggerInitialized);
    }

    // load dynamic lib in plugins dir
    {
        if (!loadPlugins()) {
            return false;
        }
    }

    // load components config files
    {
        if (!loadComponents(QDir(qApp->applicationDirPath()
                                 + QDir::separator()
                                 + DIR_COMPONENTS))) {
            return false;
        }
        expandComponents();
        qCInfo(kpf) << "Replace components referenced in component files "
                       "finished";
    }

    // load app config file
    {
        if (!loadAppConfig(FILE_APP)) {
            return false;
        }
        qApp->setObjectName(qApp->metaObject()->className());
        QString appName = rootNode.attribute(KEY_NAME, qApp->applicationDisplayName());
        if (appName.isEmpty()) {
            appName = QFileInfo(qApp->applicationFilePath()).completeBaseName();
        }
        qApp->setApplicationDisplayName(appName);
        QDir dir(qApp->applicationDirPath());
        dir.cd(DIR_STYLESHEETS);
        QFile file(dir.absoluteFilePath(rootNode.attribute(KEY_QSS)));
        if (file.exists())
        {
            if (file.open(QFile::ReadOnly | QFile::Text))
            {
                QByteArray qss = file.readAll();
                file.close();
                qApp->setStyleSheet(qss);
            }
        }
        notify(&N::appConfigLoaded);
    }

    // expand components in app config file
    {
        objectsNode = rootNode.firstChildElement(TAG_OBJECTS);
        expandComponent(objectsNode, objectsComponentsNode);
        qCInfo(kpf) << "Replace Objects components referenced in application "
                       "config file finished";

        connectionsNode = rootNode.firstChildElement(TAG_CONNECTIONS);
        expandComponent(connectionsNode, connectionsComponentsNode);
        qCInfo(kpf) << "Replace Connections components referenced in "
                       "application config file finished";

        initializationsNode = rootNode.firstChildElement(TAG_INITIALIZATIONS);
        expandComponent(initializationsNode, initializationsComponentsNode);
        qCInfo(kpf) << "Replace Initializations components referenced in "
                       "application config file finished";

        notify(&N::componentsExpanded);
    }

    // initialize objects
    {
        qCInfo(kpf) << "Start create objects";
        if (!kpfObjectImpl.createChildren(objectsNode)) {
            return false;
        }
        qCInfo(kpf) << "Create objects finished";
    }

    // initialize connections
    {
        qCInfo(kpf) << "Start initialize connections";
        if (!initConnections()) {
            return false;
        }
        qCInfo(kpf) << "Connections initialized";
    }

    // initialize initializations
    {
        qCInfo(kpf) << "Start execute initialization methods";
        if (!processInitializations()) {
            return false;
        }
        qCInfo(kpf) << "Initialization methods executed";
    }

    qCInfo(kpf) << "Initialization finished";
    inited = true;

    notify(&N::initializationFinished);

    return true;
}

Kpf::KpfCoreImpl::KpfCoreImpl()
    : mtx(QMutex::Recursive)
{
    qRegisterMetaType<QDomElement>();
    classManagerImpl.reset(new ClassManagerImpl);
    threadManagerImpl.reset(new ThreadManagerImpl);
    objectManagerImpl.reset(new ObjectManagerImpl);
    eventManagerImpl.reset(new EventManagerImpl);
    topicManagerImpl.reset(new TopicManagerImpl);
    connectionManagerImpl.reset(new ConnectionManagerImpl);
}

bool Kpf::KpfCoreImpl::loadPlugins()
{
    QMutexLocker locker(kpfMutex());

    QDir dir(qApp->applicationDirPath()
             + QDir::separator()
             + DIR_PLUGINS);
    for (const QFileInfo& fileInfo : dir.entryInfoList(PLUGIN_SUFFIX,
                                                       QDir::NoDotAndDotDot
                                                       | QDir::Files))
    {
        bool isDebugBuild = QLibraryInfo::isDebugBuild();
        bool isDebugLibrary = fileInfo.baseName().endsWith(QLatin1Char('d'));
        if (isDebugBuild != isDebugLibrary) {
            continue;
        }

        notify(&N::beginLoadLibrary, fileInfo);

        QSharedPointer<Library> library(new Library(fileInfo));
        currentLib = library;
        if (!library->library.load())
        {
            qCWarning(kpf) << "load plugin" << fileInfo.absoluteFilePath()
                           << "failed";
            continue;
        }
        qCInfo(kpf) << "load plugin" << fileInfo.absoluteFilePath()
                    << "successed";
        libraries.append(library);

        notify(&N::libraryLoaded, fileInfo);

        currentLib.clear();
    }

    return true;
}

bool Kpf::KpfCoreImpl::loadAppConfig(const QString& appFile)
{
    QMutexLocker locker(kpfMutex());

    QFileInfo fileInfo(qApp->applicationDirPath()
                       + QDir::separator()
                       + DIR_CONFIG
                       + QDir::separator()
                       + appFile);
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        qCWarning(kpf) << "Cannot open application config file"
                       << fileInfo.absoluteFilePath();
        return false;
    }
    QByteArray content = file.readAll();
//    while (!file.atEnd())
//    {
//        QString jsonStr = QString::fromUtf8(file.readLine());
//        jsonStr.replace(QRegularExpression(QStringLiteral("\\s*(.*)[\\r\\n]*")),
//                        QStringLiteral("\\1"));
//        content += jsonStr.toUtf8();
//    }
    file.close();

    QString errorMsg;
    int errorLine = 0;
    int errorColumn = 0;
    if (!appConfig.setContent(content, &errorMsg, &errorLine, &errorColumn))
    {
        qCWarning(kpf)
                << "Application config file" << fileInfo.absoluteFilePath()
                << "parse failed at line" << errorLine
                << "column" << errorColumn
                << ", error reason" << errorMsg;
        return false;
    }

    rootNode = appConfig.documentElement();

    qCInfo(kpf) << "Application config file loaded";

    return true;
}

bool Kpf::KpfCoreImpl::loadComponents(const QDir& dir)
{
    QMutexLocker locker(kpfMutex());

    qCInfo(kpf) << "Start load components in" << dir.absolutePath();

    for (const QFileInfo& fileInfo : dir.entryInfoList(QDir::NoDotAndDotDot
                                                       | QDir::Dirs
                                                       | QDir::Files))
    {

        if (fileInfo.isDir()) {
            loadComponents(QDir(fileInfo.absoluteFilePath()));
            continue;
        }

        if (fileInfo.suffix().toLower() != CONFIG_FILE_SUFFIX) {
            continue;
        }

        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            continue;
        }
        QByteArray content = file.readAll();
//        while (!file.atEnd())
//        {
//            QString contentStr = QString::fromUtf8(file.readLine());
//            contentStr.replace(QRegularExpression(
//                                   QStringLiteral("\\s*(.*)[\\r\\n]*")),
//                               QStringLiteral("\\1"));
//            content += contentStr.toUtf8();
//        }
        file.close();

        QString errorMsg;
        int errorLine = 0;
        int errorColumn = 0;
        QDomDocument doc;
        if (!doc.setContent(content, &errorMsg, &errorLine, &errorColumn))
        {
            qCWarning(kpf)
                    << "Component file" << fileInfo.absoluteFilePath()
                    << "parse failed at line" << errorLine
                    << "column" << errorColumn
                    << ", error reason" << errorMsg;
            continue;
        }
        componentsConfig.insert(fileInfo.completeBaseName(), doc);

        QDomElement root = doc.documentElement();
        auto loadComponentNode =
                [&root, fileInfo, this](const QString& tagName,
                QMap<QString, QDomElement>& map)
        {
            QDomElement node = root.firstChildElement(tagName);
            if (node.isNull()) {
                return;
            }
            for (QDomElement component = node.firstChildElement(TAG_COMPONENT);
                 !component.isNull();
                 component = component.nextSiblingElement(TAG_COMPONENT))
            {
                QString name = component.attribute(KEY_NAME);
                if (name.isEmpty()) {
                    continue;
                }
                QDomElement child = component.firstChildElement();
                if (child.isNull()) {
                    continue;
                }
                qCInfo(kpf) << "Component loaded:" << name;
                map.insert(name, child);
                notify(&N::componentLoaded, fileInfo);
            }
        };
        loadComponentNode(TAG_OBJECTS, objectsComponentsNode);
        loadComponentNode(TAG_CONNECTIONS, connectionsComponentsNode);
        loadComponentNode(TAG_INITIALIZATIONS, initializationsComponentsNode);
    }

    qCInfo(kpf) << "Components in" << dir.absolutePath() << "load finished";

    return true;
}

void Kpf::KpfCoreImpl::expandComponents()
{
    auto expandComponentsMap = [this](QMap<QString, QDomElement>& map){
        for (auto it = map.begin(); it != map.end(); ++it)
        {
            expandComponent(it.value(), map);
        }
    };
    QMutexLocker locker(kpfMutex());
    expandComponentsMap(objectsComponentsNode);
    qCInfo(kpf) << "Replace Object components referenced in compoent config "
                   "files finished";
    expandComponentsMap(connectionsComponentsNode);
    qCInfo(kpf) << "Replace Connection components referenced in compoent "
                   "config files finished";
    expandComponentsMap(initializationsComponentsNode);
    qCInfo(kpf) << "Replace Initialization components referenced in compoent "
                   "config files finished";
}

void Kpf::KpfCoreImpl::expandComponent(QDomElement& node, QMap<QString, QDomElement>& map)
{
    auto copyNode = [](QDomElement& dst, const QDomElement& src){
        dst.setTagName(src.tagName());
        QDomNamedNodeMap attributes = src.attributes();
        for (int i = 0; i < attributes.count(); ++i)
        {
            QDomAttr attr = attributes.item(i).toAttr();
            dst.setAttribute(attr.name(), attr.value());
        }
        for (QDomElement child = src.firstChildElement();
             !child.isNull();
             child = child.nextSiblingElement())
        {
            dst.appendChild(child.cloneNode(true));
        }
    };

    QMutexLocker locker(kpfMutex());

    for (QDomElement child = node.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement())
    {
        if (child.tagName() == TAG_COMPONENT)
        {
            QString componentName = child.attribute(KEY_COMPONENT);
            auto it = map.find(componentName);
            if (it == map.end()) {
                continue;
            }
            copyNode(child, it.value());
        }
        expandComponent(child, map);
    }
}

bool Kpf::KpfCoreImpl::initConnections()
{
    QMutexLocker locker(kpfMutex());

    for (QDomElement connection = connectionsNode
         .firstChildElement(TAG_CONNECTION);
         !connection.isNull();
         connection = connection.nextSiblingElement(TAG_CONNECTION))
    {
        kpfConnectionImpl.createConnection(connection);
    }

    return true;
}

bool Kpf::KpfCoreImpl::processInitializations()
{
    QMutexLocker locker(kpfMutex());

    for (QDomElement initialization = initializationsNode
         .firstChildElement(TAG_INITIALIZATION);
         !initialization.isNull();
         initialization = initialization.nextSiblingElement(TAG_INITIALIZATION))
    {
        QString objectName = initialization.attribute(KEY_OBJECT);
        QByteArray methodName = initialization.attribute(KEY_METHO).toUtf8();
        methodName = QMetaObject::normalizedSignature(methodName);

        QSharedPointer<ObjectImpl> object = kpfObject.findObject(objectName)
                                            .toStrongRef()
                                            .staticCast<ObjectImpl>();
        if (!object)
        {
            qCWarning(kpf) << "Initialization failed for object" << objectName
                           << "with method" << methodName
                           << ": cannot find object";
            continue;
        }
        const QMetaObject* metaObject = object->object->metaObject();

        QMetaMethod method;
        bool find = false;
        for (int i = metaObject->methodCount() - 1; i >= 0; --i)
        {
            method = metaObject->method(i);
            if (method.methodSignature() == methodName)
            {
                find = true;
                break;
            }
        }
        if (!find)
        {
            qCWarning(kpf) << "Initialization failed for object" << objectName
                           << "with method" << methodName
                           << ": cannot find method";
            continue;
        }

        bool ok = false;
        InvokeMethodSyncHelper(object->object, method).invoke(&ok);
        if (!ok)
        {
            qCWarning(kpf) << "Initialization failed for object" << objectName
                           << "with method" << methodName
                           << ": method execute failed";
            continue;
        }

        qCInfo(kpf) << "Initialization for object" << objectName
                    << "with method" << methodName << "successed";
    }
    return true;
}

void Kpf::KpfCoreImpl::atExit()
{
    QMutexLocker locker(kpfMutex());

    isClosingDown = true;

    notify(&N::aboutToQuit);

    connectionManagerImpl.reset(nullptr);
    notify(&N::connectionManagerDestroyed);

    topicManagerImpl.reset(nullptr);
    notify(&N::topicManagerDestroyed);

    eventManagerImpl.reset(nullptr);
    notify(&N::eventManagerDestroyed);

    objectManagerImpl.reset(nullptr);
    notify(&N::objectManagerDestroyed);

    threadManagerImpl.reset(nullptr);
    notify(&N::threadManagerDestroyed);

    classManagerImpl.reset(nullptr);
    notify(&N::classManagerDestroyed);

    while (!libraries.isEmpty())
    {
        libraries.pop_back();
    }
    notify(&N::allLibraryUnloaded);

    notify(&N::quitFinished);

    qDeleteAll(notifiers);
    notifiers.clear();
}
