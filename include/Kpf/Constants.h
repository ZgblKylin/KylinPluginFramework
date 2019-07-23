#pragma once
#include <Kpf/Common.h>

namespace Kpf {
// 所有框架内置的配置文件关键字
static const QString TAG_COMPONENTS = QStringLiteral("Components");
static const QString TAG_COMPONENT = QStringLiteral("Component");
static const QString TAG_OBJECTS = QStringLiteral("Objects");
static const QString TAG_CLASS = QStringLiteral("Class");
static const QString TAG_PUBEVENT = QStringLiteral("PubEvent");
static const QString TAG_SUBEVENT = QStringLiteral("SubEvent");
static const QString TAG_CONNECTION = QStringLiteral("Connection");
static const QString TAG_CONNECTIONS = QStringLiteral("Connections");
static const QString TAG_INITIALIZATIONS = QStringLiteral("Initializations");
static const QString TAG_INITIALIZATION = QStringLiteral("Initialization");
static const QString KEY_COMPONENT = QStringLiteral("component");
static const QString KEY_CLASS = QStringLiteral("class");
static const QString KEY_NAME = QStringLiteral("name");
static const QString KEY_TEXT = QStringLiteral("text");
static const QString KEY_QSS = QStringLiteral("qss");
static const QString KEY_PARENT = QStringLiteral("parent");
static const QString KEY_SUBTHREAD = QStringLiteral("subThread");
static const QString KEY_THREADNAME = QStringLiteral("threadName");
static const QString KEY_EVENTID = QStringLiteral("eventId");
static const QString KEY_TOPIC = QStringLiteral("topic");
static const QString KEY_SENDER = QStringLiteral("sender");
static const QString KEY_SIGNAL = QStringLiteral("signal");
static const QString KEY_RECEIVER = QStringLiteral("receiver");
static const QString KEY_SLOT = QStringLiteral("slot");
static const QString KEY_TYPE = QStringLiteral("type");
static const QString KEY_OBJECT = QStringLiteral("object");
static const QString KEY_METHO = QStringLiteral("method");
static const QString KEY_ALREADYEXIST = QStringLiteral("alreadyExist");

// 信号槽连接中连接方式对应字符串
static const QMap<QString, Qt::ConnectionType> ConnectionTypes =
{
    { QStringLiteral("Auto"), Qt::AutoConnection },
    { QStringLiteral("Direct"), Qt::DirectConnection },
    { QStringLiteral("Queued"), Qt::QueuedConnection },
    { QStringLiteral("BlockingQueued"), Qt::BlockingQueuedConnection }
};

// 初始化函数签名，返回为false，输入参数可加入const和&修饰
static QByteArray INIT_METHOD = "init(QDomElement)";

// 框架相关的文件和文件夹
static const QString DIR_PLUGINS = QStringLiteral("plugins");
static const QStringList PLUGIN_SUFFIX = {
    QStringLiteral("*.dll"),
    QStringLiteral("*.so"),
    QStringLiteral("*.dylib")
};
static const QString DIR_CONFIG = QStringLiteral("config");
static const QString FILE_APP = QStringLiteral("app.xml");
static const QString DIR_COMPONENTS = QStringLiteral("components");
static const QString DIR_STYLESHEETS = QStringLiteral("stylesheets");
static const QString CONFIG_FILE_SUFFIX = QStringLiteral("xml");
} // namespace Kpf
