#ifndef LogFILESAVETASK_H
#define LogFILESAVETASK_H

#include "../Log.h"
#include "LogFileSaverBase.h"

namespace Log {
namespace Impl {
class LogFileSaveTaskPrivate;
// base class for save log messages into file
class LogFileSaveTask : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool subThread READ subThread)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QString dir READ dir WRITE setDir)
    Q_PROPERTY(qint64 maxFileSize READ maxFileSize WRITE setMaxFileSize)
    Q_PROPERTY(QString format READ format WRITE setFormat)

    Q_DECLARE_PRIVATE(LogFileSaveTask)
    LogFileSaveTaskPrivate* d_ptr;

public:
    LogFileSaveTask(const QString& category, LogFileSaverBase* parent = 0);
    virtual ~LogFileSaveTask();

    virtual bool subThread() const;

    QString category() const;

    QString dir() const;
    void setDir(const QString& path);

    qint64 maxFileSize() const;
    void setMaxFileSize(qint64 size);

    QString format() const;
    void setFormat(const QString& str);

    // record log message into file
    void record(const QSharedPointer<Log::Impl::LogMessage> message);

protected:

    // save text into file
    virtual void save(const QByteArray& text) = 0;

    QFile* file; // log file

private:
    friend class LogFileSaverBase;
    friend class LogFileSaverBasePrivate;
};
} // namespace Log
} // namespace Impl

#endif // LogFILESAVETASK_H
