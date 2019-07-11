#pragma once
#include <Kpf/Kpf.h>
#include "EventImpl.h"

namespace Kpf {
struct TopicImpl : public Topic
{
    TopicImpl();
    virtual ~TopicImpl();

    static QSharedPointer<TopicImpl> create(const QString& name);
};

class TopicManagerImpl : public TopicManager
{
    Q_OBJECT
public:
    TopicManagerImpl();
    virtual ~TopicManagerImpl();

    static TopicManagerImpl& instance();

    // TopicManager interface
    virtual QStringList topicNames() const override;
    virtual QWeakPointer<Topic> findTopic(const QString& name) const override;

    QSharedPointer<TopicImpl> addTopic(const QString& name);

private:
    QStack<QString> names;
    QMap<QString, QSharedPointer<TopicImpl>> topics;
};

} // namespace Kpf
#define kpfTopicImpl Kpf::TopicManagerImpl::instance()
