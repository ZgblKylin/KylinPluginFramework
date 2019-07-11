#pragma once
#include <Kpf/Kpf.h>
#include "EventImpl.h"

namespace Kpf {
struct PostEvent : public QEvent
{
    PostEvent(const QVariantList& args);
    virtual ~PostEvent();

    static const int typeValue;

    QList<ObjectEvent*> eventObjects;
    QVariantList args;
};

class EventBus : public QObject
{
    Q_OBJECT
public:
    EventBus();
    virtual ~EventBus();

protected:
    virtual bool event(QEvent* e);
};
} // namespace Kpf
