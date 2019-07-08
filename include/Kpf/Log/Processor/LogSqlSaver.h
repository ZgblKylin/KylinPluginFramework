#ifndef LOGSQLSAVER_H
#define LOGSQLSAVER_H

#include "../Log.h"

#ifdef LOGSQLSAVER_LIBRARY
# define LOGSQLSAVER_SHARED_EXPORT Q_DECL_EXPORT
#elif defined(LOGSQLSAVER_USE_CODE)
# define LOGSQLSAVER_SHARED_EXPORT
#else
# define LOGSQLSAVER_SHARED_EXPORT Q_DECL_IMPORT
#endif

// CREATE TABLE [Log](
// [category] TEXT,
// [time] DATETIME NOT NULL,
// [level] TINYINT NOT NULL,
// [pid] BIGINT NOT NULL,
// [threadid] BIGINT NOT NULL,
// [threadptr] BIGINT NOT NULL,
// [file] TEXT NOT NULL,
// [line] INT NOT NULL,
// [function] TEXT NOT NULL,
// [message] TEXT NOT NULL);

namespace Log {
namespace Impl {
class LogSqlSaverPrivate;

/**
 * @brief 日志数据库存储组件，将日志存储至SQL数据库
 */
class LOGSQLSAVER_SHARED_EXPORT LogSqlSaver : public QObject, public AbstractLogProcessor
{
    Q_OBJECT
    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)

public:
    explicit LogSqlSaver(QObject* parent = nullptr);
    virtual ~LogSqlSaver();

    // AbstractLogProcessor interface
    Q_SIGNAL virtual void filterChanged(LogLevel logLevel) override;
    Q_SIGNAL virtual void categoryFiltersChanged(const categoryFilter_t& filters) override;
    Q_SIGNAL virtual void categoryFilterChanged(const QString& category, LogLevel logLevel) override;

protected:
    /**
     * @brief process 新增日志接口，用于接受来自日志引擎的推送
     * @param message 日志条目
     */
    virtual void process(const QSharedPointer<LogMessage>& message) final;

private:
    Q_DECLARE_PRIVATE(LogSqlSaver)
    LogSqlSaverPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGSQLSAVER_H
