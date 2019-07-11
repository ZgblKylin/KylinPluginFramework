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

    static bool inited = false;
    if (inited) {
        return true;
    }

    notify(&N::beginInitialization);

    QSharedPointer<Thread> defaultThread = kpfThread.defaultThread()
                                           .toStrongRef();
    defaultThread->thread = qApp->thread();
    defaultThread->eventBus->moveToThread(defaultThread->thread);

    QObject::connect(qApp, &QCoreApplication::aboutToQuit,
                     qApp, [this]{atExit();},
                     Qt::DirectConnection);

    qApp->setObjectName(qApp->metaObject()->className());

    QByteArray env = qgetenv(PATH_STR);
    if (!env.isEmpty()) {
        env += ';';
    }
    env += QDir::toNativeSeparators(qApp->applicationDirPath()
                                    + QDir::separator()
                                    + DIR_PLUGINS);
    qputenv(PATH_STR, env);

    kpfObjectImpl.createObject(QStringLiteral("debugging"),
                               QStringLiteral("Debugging"));

    initLogger(argc, argv);
    notify(&N::loggerInitialized);

    if (!loadPlugins()) {
        return false;
    }

    if (!loadComponent(QDir(qApp->applicationDirPath()
                            + QDir::separator()
                            + DIR_COMPONENTS))) {
        return false;
    }
    for (auto it = componentsNode.begin();
         it != componentsNode.end();
         ++it)
    {
        expandComponents(it.value());
    }
    kpfCInformation("Kpf") << "Replace components referenced in component files finished";

    if (!loadAppConfig()) {
        return false;
    }
    notify(&N::appConfigLoaded);

    QJsonValueRef value = rootNode[TAG_OBJECTS];
    expandComponents(value);
    objectsNode = value.toArray();
    kpfCInformation("Kpf") << "Replace Objects components referenced in application config file finished";

    value = rootNode[TAG_CONNECTIONS];
    expandComponents(value);
    connectionsNode = value.toArray();
    kpfCInformation("Kpf") << "Replace Connections components referenced in application config file finished";

    value = rootNode[TAG_INITIALIZATIONS];
    expandComponents(value);
    initializationsNode = value.toArray();
    kpfCInformation("Kpf") << "Replace Initializations components referenced in application config file finished";

    notify(&N::componentsExpanded);

    kpfCInformation("Kpf") << "Start create objects";
    if (!kpfObjectImpl.createChildren(objectsNode)) {
        return false;
    }
    kpfCInformation("Kpf") << "Create objects finished";

    kpfCInformation("Kpf") << "Start initialize connections";
    if (!initConnections()) {
        return false;
    }
    kpfCInformation("Kpf") << "Connections initialized";

    kpfCInformation("Kpf") << "Start execute initialization methods";
    if (!processInitializations()) {
        return false;
    }
    kpfCInformation("Kpf") << "Initialization methods executed";

    kpfCInformation("Kpf") << "Initialization finished";
    inited = true;

    notify(&N::initializationFinished);

    return true;
}

Kpf::KpfCoreImpl::KpfCoreImpl()
    : mtx(QMutex::Recursive)
{
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
            kpfCWarning("Kpf") << "load plugin" << fileInfo.absoluteFilePath() << "failed";
            continue;
        }
        kpfCInformation("Kpf") << "load plugin" << fileInfo.absoluteFilePath() << "successed";
        libraries.append(library);

        notify(&N::libraryLoaded, fileInfo);

        currentLib.clear();
    }

    return true;
}

bool Kpf::KpfCoreImpl::loadAppConfig()
{
    QMutexLocker locker(kpfMutex());

    QFileInfo fileInfo(qApp->applicationDirPath()
                       + QDir::separator()
                       + DIR_CONFIG
                       + QDir::separator()
                       + FILE_APP);
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        kpfCWarning("Kpf") << "Cannot open application config file"
                           << fileInfo.absoluteFilePath();
        return false;
    }
    QByteArray json;
    while (!file.atEnd())
    {
        QString jsonStr = QString::fromUtf8(file.readLine());
        jsonStr.replace(QRegularExpression(QStringLiteral("\\s*(.*)[\\r\\n]*")),
                        QStringLiteral("\\1"));
        json += jsonStr.toUtf8();
    }
    file.close();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError)
    {
        int offset = std::max(0, error.offset - 20);
        kpfCWarning("Kpf") << "Application config file" << fileInfo.absoluteFilePath() << "parse failed"
                           << ", error reason" << error.errorString()
                           << ", error position" << error.offset
                           << ", error context:" << json.mid(offset, 40).constData();
        return false;
    }

    rootNode = doc.object();

    kpfCInformation("Kpf") << "Application config file loaded";

    return true;
}

bool Kpf::KpfCoreImpl::loadComponent(const QDir& dir)
{
    QMutexLocker locker(kpfMutex());

    kpfCInformation("Kpf") << "Start load components in" << dir.absolutePath();

    for (const QFileInfo& fileInfo : dir.entryInfoList(QDir::NoDotAndDotDot
                                                       | QDir::Dirs
                                                       | QDir::Files))
    {

        if (fileInfo.isDir()) {
            loadComponent(QDir(fileInfo.absoluteFilePath()));
            continue;
        }

        if (fileInfo.suffix().toLower() != FILE_SUFFIX_JSON) {
            continue;
        }

        QFile file(fileInfo.absoluteFilePath());
        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            continue;
        }
        QByteArray json;
        while (!file.atEnd())
        {
            QString jsonStr = QString::fromUtf8(file.readLine());
            jsonStr.replace(QRegularExpression(QStringLiteral("\\s*(.*)[\\r\\n]*")),
                            QStringLiteral("\\1"));
            json += jsonStr.toUtf8();
        }
        file.close();

        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(json, &error);
        if (error.error != QJsonParseError::NoError)
        {
            int offset = std::max(0, error.offset - 50);
            kpfCWarning("Kpf") << "Component file" << fileInfo.absoluteFilePath() << "parse failed"
                               << ", error reason" << error.errorString()
                               << ", error position" << error.offset
                               << ", error context:" << json.mid(offset, 100).constData();
            continue;
        }

        QJsonObject root = doc.object().value(TAG_COMPONENTS).toObject();
        for (auto it = root.constBegin(); it != root.constEnd(); ++it)
        {
            switch (it.value().type())
            {
            case QJsonValue::Object:
            case QJsonValue::Array:
                kpfCInformation("Kpf") << "Component loaded:" << it.key();
                componentsNode.insert(it.key(),
                                      it.value());
                notify(&N::componentLoaded, fileInfo);
                break;
            default:
                break;
            }
        }
    }

    kpfCInformation("Kpf") << "Components in" << dir.absolutePath() << "load finished";

    return true;
}

template<typename Value>
void Kpf::KpfCoreImpl::expandComponents(Value& value)
{
    QMutexLocker locker(kpfMutex());

    switch (value.type())
    {
    case QJsonValue::Object:
    {
        QJsonObject object = value.toObject();
        if (object.contains(TAG_COMPONENT)) {
            expandObjectComponent(object);
        }
        for (QJsonObject::iterator it = object.begin(); it != object.end(); ++it)
        {
            QString key = it.key();
            QJsonValueRef ref = it.value();
            expandComponents(ref);
        }
        value = object;
    }
        break;

    case QJsonValue::Array:
    {
        QJsonArray array = value.toArray();

        QStack<QPair<int, QJsonArray>> arraysToBeReplaced;
        for (int i = 0; i < array.count(); ++i)
        {
            QJsonValueRef ref = array[i];
            if (ref.isString())
            {
                QJsonArray ret = expandStringComponent(ref);
                if (!ret.isEmpty()) {
                    arraysToBeReplaced.push(qMakePair(i, ret));
                }
            }
            else
            {
                expandComponents(ref);
            }
        }

        while (!arraysToBeReplaced.isEmpty())
        {
            const QPair<int, QJsonArray>& arrayToBeReplaced = arraysToBeReplaced.pop();
            array.removeAt(arrayToBeReplaced.first);
            for (int i = arrayToBeReplaced.second.count() - 1; i >= 0; --i)
            {
                array.insert(arrayToBeReplaced.first,
                             arrayToBeReplaced.second.at(i));
            }
        }

        value = array;
    }
        break;

    default:
        break;
    }
}

bool Kpf::KpfCoreImpl::initConnections()
{
    QMutexLocker locker(kpfMutex());

    for (const QJsonValue& value : connectionsNode)
    {
        if (!value.isObject()) {
            continue;
        }
        QJsonObject connectionConfig = value.toObject();
        kpfConnectionImpl.createConnection(connectionConfig);
    }

    return true;
}

bool Kpf::KpfCoreImpl::processInitializations()
{
    QMutexLocker locker(kpfMutex());

    for (int i = 0; i < initializationsNode.count(); ++i)
    {
        if (!initializationsNode.at(i).isObject()) {
            continue;
        }
        QJsonObject initialization = initializationsNode.at(i).toObject();
        QString objectName = initialization.value(TAG_OBJECT).toString();
        QByteArray methodName = initialization.value(TAG_METHOD).toString().toUtf8();
        methodName = QMetaObject::normalizedSignature(methodName);

        QSharedPointer<ObjectImpl> object = kpfObject.findObject(objectName)
                                            .toStrongRef()
                                            .staticCast<ObjectImpl>();
        if (!object)
        {
            kpfCWarning("Kpf") << "Initialization failed for object" << objectName
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
            kpfCWarning("Kpf") << "Initialization failed for object" << objectName
                               << "with method" << methodName
                               << ": cannot find method";
            continue;
        }

        bool ok = false;
        InvokeMethodSyncHelper(object->object, method).invoke(&ok);
        if (!ok)
        {
            kpfCWarning("Kpf") << "Initialization failed for object" << objectName
                               << "with method" << methodName
                               << ": method execute failed";
            continue;
        }

        kpfCInformation("Kpf") << "Initialization for object" << objectName
                               << "with method" << methodName
                               << "successed";
    }
    return true;
}

void Kpf::KpfCoreImpl::expandObjectComponent(QJsonObject& element)
{
    QMutexLocker locker(kpfMutex());

    QString name = element.value(TAG_COMPONENT).toString();
    if (!componentsNode.contains(name)) {
        return;
    }

    QJsonValue component = componentsNode.value(name);
    if (!component.isObject()) {
        return;
    }

    element.remove(TAG_COMPONENT);
    QJsonObject object = component.toObject();
    for (auto it = object.constBegin(); it != object.constEnd(); ++it)
    {
        if (!element.contains(it.key())) {
            element[it.key()] = it.value();
        }
    }

    // 递归调用，避免错过Component里嵌套使用的Component
    for (QJsonObject::iterator it = element.begin(); it != element.end(); ++it)
    {
        QJsonValueRef ref = it.value();
        expandComponents(ref);
    }
}

QJsonArray Kpf::KpfCoreImpl::expandStringComponent(QJsonValueRef& ref)
{
    QMutexLocker locker(kpfMutex());

    QString componentName = ref.toString();
    QJsonValue component = componentsNode.value(componentName);
    switch (component.type())
    {
    case QJsonValue::Object:
        expandComponents(component); //< 递归子项
        ref = component;
        break;
    case QJsonValue::Array:
        expandComponents(component); //< 递归子项
        return component.toArray();
    default:
        break;
    }
    return QJsonArray();
}

void Kpf::KpfCoreImpl::atExit()
{
    QMutexLocker locker(kpfMutex());

    isClosingDown = true;

    notify(&N::aboutToQuit);

    QStack<QSharedPointer<Library>> temp;
    for (auto it = libraries.begin(); it != libraries.end(); ++it)
    {
        if ((*it)->objects.isEmpty())
        {
            temp.push(*it);
            it = libraries.erase(it);
            --it;
        }
    }

    notify(&N::allObjectsDestroyed);

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

    while (!libraries.isEmpty()) {}
    while (!temp.isEmpty()) {
        temp.pop();
    }
    notify(&N::allLibraryUnloaded);

    notify(&N::quitFinished);

    qDeleteAll(notifiers);
    notifiers.clear();
}
