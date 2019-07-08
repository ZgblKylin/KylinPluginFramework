#ifndef LOGFILEMMAPSAVER_H
#define LOGFILEMMAPSAVER_H

#include "LogFileSaverBase.h"

namespace Log {
namespace Impl {
class LogFileMmapSaverPrivate;
class LOGFILESAVER_SHARED_EXPORT LogFileMmapSaver : public LogFileSaverBase
{
    Q_OBJECT
    // Mmap block size. Default is 1024
    Q_PROPERTY(qint64 mapSize READ mapSize WRITE setMapSize)

public:
    explicit LogFileMmapSaver(QObject* parent = nullptr);
    virtual ~LogFileMmapSaver();

    qint64 mapSize() const;
    void setMapSize(qint64 size);

protected:
    // LogFileSaverBase interface
    virtual LogFileSaveTask* createTask(const QString& category) const override final;

private:
    Q_DECLARE_PRIVATE(LogFileMmapSaver)
    LogFileMmapSaverPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGFILEMMAPSAVER_H
