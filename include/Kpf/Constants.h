/* Constant.h
 * 提供框架内置的各项常量
 */
#ifndef KPF_CONSTANTS_H
#define KPF_CONSTANTS_H

#include <Kpf/Common.h>

namespace Kpf {
// 所有框架内置的json关键字
static const QString TAG_COMPONENTS = QStringLiteral("Components");
static const QString TAG_COMPONENT = QStringLiteral("Component");
static const QString TAG_OBJECTS = QStringLiteral("Objects");
static const QString TAG_CLASS = QStringLiteral("Class");
static const QString TAG_NAME = QStringLiteral("Name");
static const QString TAG_PARENT = QStringLiteral("Parent");
static const QString TAG_SUBTHREAD = QStringLiteral("SubThread");
static const QString TAG_THREADNAME = QStringLiteral("ThreadName");
static const QString TAG_PUBEVENT = QStringLiteral("PubEvent");
static const QString TAG_SUBEVENT = QStringLiteral("SubEvent");
static const QString TAG_EVENTID = QStringLiteral("EventId");
static const QString TAG_TOPIC = QStringLiteral("Topic");
static const QString TAG_CONNECTIONS = QStringLiteral("Connections");
static const QString TAG_SENDER = QStringLiteral("Sender");
static const QString TAG_SIGNAL = QStringLiteral("Signal");
static const QString TAG_RECEIVER = QStringLiteral("Receiver");
static const QString TAG_SLOT = QStringLiteral("Slot");
static const QString TAG_TYPE = QStringLiteral("Type");
static const QString TAG_INITIALIZATIONS = QStringLiteral("Initializations");
static const QString TAG_OBJECT = QStringLiteral("Object");
static const QString TAG_METHOD = QStringLiteral("Method");

// 信号槽连接中连接方式对应字符串
static const QMap<QString, Qt::ConnectionType> ConnectionTypes =
{
    { QStringLiteral("Auto"), Qt::AutoConnection },
    { QStringLiteral("Direct"), Qt::DirectConnection },
    { QStringLiteral("Queued"), Qt::QueuedConnection },
    { QStringLiteral("BlockingQueued"), Qt::BlockingQueuedConnection }
};

// 初始化函数签名，返回为false，输入参数可加入const和&修饰
static QByteArray INIT_METHOD = "init(QJsonObject)";

// 框架相关的文件和文件夹
static const QString DIR_PLUGINS = QStringLiteral("plugins");
static const QStringList PLUGIN_SUFFIX = {
    QStringLiteral("*.dll"),
    QStringLiteral("*.so"),
    QStringLiteral("*.dylib")
};
static const QString DIR_CONFIG = QStringLiteral("config");
static const QString FILE_APP = QStringLiteral("app.json");
static const QString DIR_COMPONENTS = QStringLiteral("components");
static const QString FILE_SUFFIX_JSON = QStringLiteral("json");
} // namespace Kpf

#endif // KPF_CONSTANTS_H
