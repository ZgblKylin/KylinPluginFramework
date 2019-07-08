#include "TopicImpl.h"
#include "KpfPrivate.h"

Kpf::Topic::~Topic()
{
}

Kpf::Topic::Topic()
{
}

Kpf::TopicImpl::TopicImpl()
{
}

Kpf::TopicImpl::~TopicImpl()
{
}

QSharedPointer<Kpf::TopicImpl> Kpf::TopicImpl::create(const QString& name)
{
    QSharedPointer<TopicImpl> ret = QSharedPointer<TopicImpl>::create();
    ret->name = name;
    return ret;
}

Kpf::TopicManager& Kpf::TopicManager::instance()
{
    return kpfCoreImpl.topicManager();
}

Kpf::TopicManagerImpl::TopicManagerImpl()
{
}

Kpf::TopicManagerImpl::~TopicManagerImpl()
{
    QMutexLocker locker(kpfMutex());
    while (!names.isEmpty()) {
        topics.remove(names.pop());
    }
}

Kpf::TopicManagerImpl& Kpf::TopicManagerImpl::instance()
{
    return kpfCoreImpl.topicManager();
}

QStringList Kpf::TopicManagerImpl::topicNames() const
{
    QMutexLocker locker(kpfMutex());
    return topics.keys();
}

QWeakPointer<Kpf::Topic> Kpf::TopicManagerImpl::findTopic(const QString& name) const
{
    QMutexLocker locker(kpfMutex());
    return topics.value(name).staticCast<Topic>();
}

QSharedPointer<Kpf::TopicImpl> Kpf::TopicManagerImpl::addTopic(const QString& name)
{
    QMutexLocker locker(kpfMutex());
    QSharedPointer<TopicImpl> topic = findTopic(name)
                                      .toStrongRef()
                                      .staticCast<TopicImpl>();
    if (topic) {
        return topic;
    }

    topic = TopicImpl::create(name);
    names.push(name);
    topics.insert(name, topic);
    return topic;
}
