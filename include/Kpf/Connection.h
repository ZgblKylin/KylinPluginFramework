#pragma once
#include <Kpf/Common.h>

// ======== API声明 ========

// 获取事件管理器对象，等同于Kpf::ConnectionManager::instance()
#define kpfConnection

namespace Kpf {
// 信号槽连接信息对象
struct KPFSHARED_EXPORT Connection
{
    virtual ~Connection();

    QString sender;             // 发送方对象名
    QByteArray signalName;      // 信号方法
    QString receiver;           // 接收方对象名
    QByteArray slotName;        // 槽方法
    Qt::ConnectionType type;    // 连接方式
};

struct IConnectionNotifier
{
    virtual ~IConnectionNotifier() = default;

    virtual void connectionCreated(const QString& sender, const QByteArray& signal, const QString& receiver, const QByteArray& slot, Qt::ConnectionType type = Qt::AutoConnection) { Q_UNUSED(sender) Q_UNUSED(signal) Q_UNUSED(receiver) Q_UNUSED(slot) Q_UNUSED(type) }
    virtual void connectionRemoved(const QString& sender, const QByteArray& signal, const QString& receiver, const QByteArray& slot, Qt::ConnectionType type = Qt::AutoConnection) { Q_UNUSED(sender) Q_UNUSED(signal) Q_UNUSED(receiver) Q_UNUSED(slot) Q_UNUSED(type) }
};

class KPFSHARED_EXPORT ConnectionManager : public QObject, virtual public INotifyManager<IConnectionNotifier>
{
    Q_OBJECT
public:
    virtual ~ConnectionManager() = default;

    static ConnectionManager& instance();

    /**
     * @brief createConnection 创建信号槽
     * @param sender 发送方对象名
     * @param signal 信号函数签名
     * @param receiver 接收方对象名
     * @param slot 槽函数签名
     * @param type 连接类型
     * @return 创建的信号槽连接信息对象，若创建失败，返回nullptr
     */
    virtual const QWeakPointer<Connection> createConnection(const QString& sender, const QString& signal, const QString& receiver, const QString& slot, Qt::ConnectionType type = Qt::AutoConnection) = 0;
    // 根据配置节点创建信号槽，格式详见app.json
    virtual const QWeakPointer<Connection> createConnection(const QDomElement& config) = 0;
    // 销毁信号槽连接
    virtual void removeConnection(QWeakPointer<Connection> connection) = 0;
    // 获取当前已建立的所有信号槽连接
    virtual QList<QWeakPointer<Connection>> connections(const QString& name) const = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfConnection
#define kpfConnection Kpf::ConnectionManager::instance()
