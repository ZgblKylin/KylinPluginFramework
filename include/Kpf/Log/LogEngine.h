#ifndef LOGENGINE_H
#define LOGENGINE_H

#include "LogCommon.h"
#include "LogStream.h"
#include "AbstractLogProcessor.h"

#ifdef LOGENGINE_LIBRARY
# define LOGENGINE_SHARED_EXPORT Q_DECL_EXPORT
#else
# define LOGENGINE_SHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Log {
namespace Impl {
class LOGENGINE_SHARED_EXPORT ILogEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)

public:
    virtual ~ILogEngine() = default;

    virtual LogLevel filter() const = 0;
    virtual void setFilter(LogLevel logLevel) = 0;
    Q_SIGNAL void filterChanged(LogLevel logLevel);

    using categoryFilter_t = AbstractLogProcessor::categoryFilter_t;
    virtual categoryFilter_t categoryFilters() const = 0;
    virtual void setCategoryFilters(const categoryFilter_t& filters) = 0;
    Q_SIGNAL void categoryFiltersChanged(const categoryFilter_t& filters);

    virtual LogLevel categoryFilter(const QString& category) const = 0;
    virtual void setCategoryFilter(QString category, LogLevel logLevel) = 0;
    Q_SIGNAL void categoryFilterChanged(const QString& category, LogLevel logLevel);

    // 判断输入日志等级是否应保留，大于等于过滤级别的日志将被保留
    virtual bool filterAcceptLogLevel(const QString& category, LogLevel logLevel) const = 0;

    /**
     * @brief getStream 生成日志输出流
     * @param category 日志分类
     * @param logLevel 日志级别
     * @param file 日志所属文件名
     * @param line 日志所属代码行号
     * @param function 日志所属函数名
     * @return 日志输出流
     */
    virtual LogStream getStream(const QString& category, LogLevel logLevel, QString file, int line, QString function) const = 0;

    Q_SIGNAL void newLog(QSharedPointer<Log::Impl::LogMessage> message);

    template<typename Processor> bool registerProcessor(Processor* processor, Qt::ConnectionType connectionType = Qt::QueuedConnection);
    virtual bool registerProcessor(AbstractLogProcessor* processor, QObject* processorObject, Qt::ConnectionType connectionType = Qt::QueuedConnection) = 0;

    template<typename Processor> void unRegisterProcessor(Processor* processor);
    virtual void unRegisterProcessor(AbstractLogProcessor* processor, QObject* processorObject) = 0;
};
class LogEnginePrivate;
class LOGENGINE_SHARED_EXPORT LogEngine : public ILogEngine
{
    Q_OBJECT

public:
    virtual ~LogEngine();

    static ILogEngine& instance();

    virtual LogLevel filter() const final;
    virtual void setFilter(LogLevel logLevel) final;

    virtual categoryFilter_t categoryFilters() const final;
    virtual void setCategoryFilters(const categoryFilter_t& filters) final;

    virtual LogLevel categoryFilter(const QString& category) const final;
    virtual void setCategoryFilter(QString category, LogLevel logLevel) final;

    virtual bool filterAcceptLogLevel(const QString& category, LogLevel logLevel) const final;

    virtual LogStream getStream(const QString& category, LogLevel logLevel, QString file, int line, QString function) const final;

    virtual bool registerProcessor(AbstractLogProcessor* processor, QObject* processorObject, Qt::ConnectionType connectionType = Qt::QueuedConnection) final;
    virtual void unRegisterProcessor(AbstractLogProcessor* processor, QObject* processorObject) final;

protected:
    Q_DECLARE_PRIVATE(LogEngine)
    LogEnginePrivate* d_ptr;
    LogEngine();
};

template<typename Processor>
bool ILogEngine::registerProcessor(Processor* processor, Qt::ConnectionType connectionType)
{
    return registerProcessor(processor, processor, connectionType);
}

template<typename Processor>
void ILogEngine::unRegisterProcessor(Processor* processor)
{
    unRegisterProcessor(processor, processor);
}
} // namespace Impl
} // namespace Log

#endif // LOGENGINE_H
