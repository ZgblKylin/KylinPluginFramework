#include "EventBus.h"
#include "KpfPrivate.h"

const int Kpf::PostEvent::typeValue = QEvent::User + 'K' + 'p' + 'f';

Kpf::PostEvent::PostEvent(const QVariantList& arguments)
    : QEvent(QEvent::Type(typeValue)),
      args(arguments)
{
}

Kpf::PostEvent::~PostEvent()
{
}

Kpf::EventBus::EventBus()
{
}

bool Kpf::EventBus::event(QEvent* e)
{
    if (e->type() != PostEvent::typeValue) {
        return QObject::event(e);
    }

    PostEvent* event = static_cast<PostEvent*>(e);
    for (ObjectEvent* eventObject : event->eventObjects)
    {
        eventObject->func(event->args);
    }
    event->accept();
    return true;
}

Kpf::EventBus::~EventBus()
{
}
