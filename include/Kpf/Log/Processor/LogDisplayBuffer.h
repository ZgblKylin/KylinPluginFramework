#ifndef LOGDISPLAYBUFFER_H
#define LOGDISPLAYBUFFER_H

#include <QtCore/QtCore>
#include "../Log.h"

#ifdef LOGDISPLAYWIDGET_LIBRARY
# define LOGDISPLAYWIDGET_SHARED_EXPORT Q_DECL_EXPORT
#elif defined(LOGDISPLAYWIDGET_USE_CODE)
# define LOGDISPLAYWIDGET_SHARED_EXPORT
#else
# define LOGDISPLAYWIDGET_SHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Log {
namespace Impl {
class ILogDisplayModel : public QAbstractTableModel
{
    /* Must add codes below when inherited
    Q_PROPERTY(int setMaxCount READ maxCount WRITE setMaxCount)
    Q_PROPERTY(quint8 filter READ filter WRITE setFilter)
    */

public:
    explicit ILogDisplayModel(QObject* parent) : QAbstractTableModel(parent) {}
    virtual ~ILogDisplayModel() = default;

    virtual int maxCount() const = 0;
    virtual void setMaxCount(int value) = 0;

    virtual LogLevel filter() const = 0;
    virtual void setFilter(LogLevel filter) = 0;

    // clear data
    virtual void clear() = 0;

    // add new log message
    virtual void newLog(QSharedPointer<LogMessage> message) = 0;
};

class LogDisplayBufferPrivate;
class LOGDISPLAYWIDGET_SHARED_EXPORT LogDisplayBuffer : public QObject, public AbstractLogProcessor
{
    Q_OBJECT

    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)

public:
    explicit LogDisplayBuffer(QObject* parent = nullptr);
    virtual ~LogDisplayBuffer();

    // AbstractLogProcessor interface
    Q_SIGNAL virtual void filterChanged(LogLevel logLevel) override;
    Q_SIGNAL virtual void categoryFiltersChanged(const categoryFilter_t& filters) override;
    Q_SIGNAL virtual void categoryFilterChanged(const QString& category, LogLevel logLevel) override;

    ILogDisplayModel* model(const QString& category);

    Q_SIGNAL void newLogRecorded(const QSharedPointer<LogMessage>& message);

protected:
    // AbstractLogProcessor interface
    virtual void process(const QSharedPointer<LogMessage>& message) final;

private:
    Q_DECLARE_PRIVATE(LogDisplayBuffer)
    LogDisplayBufferPrivate* d_ptr;
};
} // namespace Log
} // namespace Impl

#endif // LOGDISPLAYBUFFER_H
