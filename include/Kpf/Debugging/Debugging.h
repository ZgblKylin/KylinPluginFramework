#ifndef DEBUGGING_H
#define DEBUGGING_H

#include <QtCore/QtCore>
#include <functional>

struct IDebugging
{
public:
    virtual ~IDebugging() = default;

    virtual QStringList stackWalk() = 0;
    virtual QStringList dumpSystemInfo() = 0;
    virtual void dump(const QString& dir) = 0;
    virtual void registerCallback(std::function<void(int)> callback) = 0;
};

class Debugging : public QObject, public IDebugging
{
    Q_OBJECT
public:
    explicit Debugging(QObject* parent = nullptr);
    virtual ~Debugging();

    virtual QStringList stackWalk() final;
    virtual QStringList dumpSystemInfo() final;
    virtual void dump(const QString& dir) final;
    virtual void registerCallback(std::function<void(int)> callback) final;
};

#endif // DEBUGGING_H
