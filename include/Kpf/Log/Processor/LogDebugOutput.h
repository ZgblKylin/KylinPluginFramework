#ifndef LOGDEBUGOUTPUT_H
#define LOGDEBUGOUTPUT_H

#include <QtCore/QtCore>
#include "../Log.h"

#ifdef LOGDEBUGOUTPUT_LIBRARY
# define LOGDEBUGOUTPUT_SHARED_EXPORT Q_DECL_EXPORT
#elif defined(LOGDEBUGOUTPUT_USE_CODE)
# define LOGDEBUGOUTPUT_SHARED_EXPORT
#else
# define LOGDEBUGOUTPUT_SHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Log {
namespace Impl {
class LogDebugOutputPrivate;
/**
 * @brief 日志输出组件，将日志信息输出至控制台
 */
class LOGDEBUGOUTPUT_SHARED_EXPORT LogDebugOutput : public QObject, public AbstractLogProcessor
{
    Q_OBJECT

    // 日志格式，默认为Log::Impl::DefaultPattern
    Q_PROPERTY(QString format READ format WRITE setFormat)

    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)

public:
    explicit LogDebugOutput(QObject* parent = nullptr);
    virtual ~LogDebugOutput();

    QString format() const;
    void setFormat(const QString& format);

    // AbstractLogProcessor interface
    Q_SIGNAL virtual void filterChanged(LogLevel logLevel) override;
    Q_SIGNAL virtual void categoryFiltersChanged(const categoryFilter_t& filters) override;
    Q_SIGNAL virtual void categoryFilterChanged(const QString& category, LogLevel logLevel) override;

protected:
    // AbstractLogProcessor interface
    virtual void process(const QSharedPointer<LogMessage>& message) final;

private:
    Q_DECLARE_PRIVATE(LogDebugOutput)
    LogDebugOutputPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGDEBUGOUTPUT_H
