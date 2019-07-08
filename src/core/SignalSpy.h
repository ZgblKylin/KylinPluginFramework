#ifndef SIGNALSPY_H
#define SIGNALSPY_H

#include <Kpf/Kpf.h>

namespace Kpf {
class ObjectImpl;

class SignalSpy : public QObject
{
public:
    SignalSpy(QSharedPointer<ObjectImpl>& object, const QByteArray& eventName);

    ObjectEvent eventObject;

protected:
    virtual int qt_metacall(QMetaObject::Call call, int methodId, void **a) override;

private:
    QByteArray signal;
    QVariantList args;
};

} // namespace Kpf

#endif // SIGNALSPY_H
