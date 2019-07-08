#ifndef LOGFILENORMALSAVER_H
#define LOGFILENORMALSAVER_H

#include "LogFileSaverBase.h"

namespace Log {
namespace Impl {
class LogFileNormalSaverPrivate;
class LOGFILESAVER_SHARED_EXPORT LogFileNormalSaver : public LogFileSaverBase
{
    Q_OBJECT
    Q_PROPERTY(int flushCount READ flushCount WRITE setFlushCount)

public:
    explicit LogFileNormalSaver(QObject* parent = nullptr);
    virtual ~LogFileNormalSaver();

    int flushCount() const;
    void setFlushCount(int count);

protected:
    // LogFileSaverBase interface
    virtual LogFileSaveTask* createTask(const QString& category) const override final;

private:
    Q_DECLARE_PRIVATE(LogFileNormalSaver)
    LogFileNormalSaverPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGFILENORMALSAVER_H
