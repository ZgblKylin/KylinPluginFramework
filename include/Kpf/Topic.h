#pragma once
#include <Kpf/Common.h>

// ======== API声明 ========

// 获取TopicManager对象，等同于Kpf::TopicManager::instance()
#define kpfTopic

namespace Kpf {
struct ObjectEvent;

struct KPFSHARED_EXPORT Topic
{
    virtual ~Topic();

    QString name;
    QSet<ObjectEvent*> publishedEvents;
    QSet<ObjectEvent*> subscribedEvents;
    QMap<quintptr, QList<ObjectEvent*>> threadEvents;

protected:
    Topic();
};

class KPFSHARED_EXPORT TopicManager : public QObject
{
    Q_OBJECT
public:
    virtual ~TopicManager() = default;

    static TopicManager& instance();

    virtual QStringList topicNames() const = 0;
    virtual QWeakPointer<Topic> findTopic(const QString& name) const = 0;
};
} // namespace Kpf
// ======== API声明 ========

#undef kpfTopic
#define kpfTopic Kpf::TopicManager::instance()
