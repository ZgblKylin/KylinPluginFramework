#pragma once
#include <QtCore/QtCore>
#include <functional>
#include <Kpf/Kpf.h>

class CoreDump : public QObject
{
    Q_OBJECT
public:
    explicit CoreDump(QObject* parent = nullptr);
    virtual ~CoreDump();

    QStringList stackWalk();
    QStringList dumpSystemInfo();
    void dump(const QString& dir);
    void registerCallback(std::function<void(int)> callback);
};
KPF_REGISTER(CoreDump)
