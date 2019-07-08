#ifndef ABSTRACTLOGPROCESSOR_H
#define ABSTRACTLOGPROCESSOR_H

#include "LogCommon.h"
#include "LogStream.h"

namespace Log {
namespace Impl {
static const QString PatternCategory = QStringLiteral("%{category}");
static const QString PatternDateTime = QStringLiteral("%{datetime}");
static const QString PatternType = QStringLiteral("%{type}");
static const QString PatternLevel = QStringLiteral("%{level}");
static const QString PatternPID = QStringLiteral("%{pid}");
static const QString PatternThreadId = QStringLiteral("%{threadid}");
static const QString PatternThreadPtr = QStringLiteral("%{threadptr}");
static const QString PatternFile = QStringLiteral("%{file}");
static const QString PatternLine = QStringLiteral("%{line}");
static const QString PatternFunction = QStringLiteral("%{function}");
static const QString PatternMessage = QStringLiteral("%{message}");
static const QString DefaultPattern = QStringLiteral("[%1] %2 [%3(%4)] %5:%6 %7:%8 %9 - %10")
                                      .arg(PatternCategory)
                                      .arg(PatternDateTime)
                                      .arg(PatternType)
                                      .arg(PatternLevel)
                                      .arg(PatternPID)
                                      .arg(PatternThreadId)
                                      .arg(PatternFile)
                                      .arg(PatternLine)
                                      .arg(PatternFunction)
                                      .arg(PatternMessage);
static const QString PlaceHolderCategory = QStringLiteral("%{C}");
static const QString PlaceHolderDateTime = QStringLiteral("%{D}");
static const QString PlaceHolderType = QStringLiteral("%{T}");
static const QString PlaceHolderLevel = QStringLiteral("%{V}");
static const QString PlaceHolderPID = QStringLiteral("%{P}");
static const QString PlaceHolderThreadId = QStringLiteral("%{I}");
static const QString PlaceHolderThreadPtr = QStringLiteral("%{H}");
static const QString PlaceHolderFile = QStringLiteral("%{F}");
static const QString PlaceHolderLine = QStringLiteral("%{L}");
static const QString PlaceHolderFunction = QStringLiteral("%{N}");
static const QString PlaceHolderMessage = QStringLiteral("%{M}");
QString simplifyPattern(const QString& pattern);
QString processMessage(const QString& simplifiedPattern, const QSharedPointer<LogMessage>& message);

class AbstractLogProcessor
{
    /* Must add codes below when inherited
    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)
    */

public:
    virtual ~AbstractLogProcessor() = default;

    LogLevel filter() const;
    void setFilter(LogLevel logLevel);
    virtual void filterChanged(LogLevel logLevel) = 0; // Must be inherited as Q_SIGNAL

    using categoryFilter_t = QHash<QString, quint8>;
    categoryFilter_t categoryFilters() const;
    void setCategoryFilters(const categoryFilter_t& filters);
    virtual void categoryFiltersChanged(const categoryFilter_t& filters) = 0; // Must be inherited as Q_SIGNAL

    LogLevel categoryFilter(const QString& category) const;
    void setCategoryFilter(QString category, LogLevel logLevel);
    virtual void categoryFilterChanged(const QString& category, LogLevel logLevel) = 0; // Must be inherited as Q_SIGNAL

    void newLog(const QSharedPointer<LogMessage>& message);

protected:
    virtual void process(const QSharedPointer<LogMessage>& message) = 0;

private:
    LogLevel f;
    categoryFilter_t cf;
};

inline QString simplifyPattern(const QString& pattern)
{
    QString ret = pattern;
    ret.replace(PatternCategory, PlaceHolderCategory + '\x1f');
    ret.replace(PatternDateTime, PlaceHolderDateTime + '\x1f');
    ret.replace(PatternType, PlaceHolderType + '\x1f');
    ret.replace(PatternLevel, PlaceHolderLevel + '\x1f');
    ret.replace(PatternPID, PlaceHolderPID + '\x1f');
    ret.replace(PatternThreadId, PlaceHolderThreadId + '\x1f');
    ret.replace(PatternThreadPtr, PlaceHolderThreadPtr + '\x1f');
    ret.replace(PatternFile, PlaceHolderFile + '\x1f');
    ret.replace(PatternLine, PlaceHolderLine + '\x1f');
    ret.replace(PatternFunction, PlaceHolderFunction + '\x1f');
    ret.replace(PatternMessage, PlaceHolderMessage + '\x1f');
    return ret;
}

inline QString processMessage(const QString& simplifiedPattern, const QSharedPointer<LogMessage>& message)
{
    QString text = simplifiedPattern;
    text.replace(PlaceHolderCategory, message->category);
    text.replace(PlaceHolderDateTime, message->dateTime.toString(QStringLiteral("yyyy-MM-dd HH:mm:ss zzz")));
    text.replace(PlaceHolderType, message->level.toString());
    text.replace(PlaceHolderLevel, QString::number(message->level));
    text.replace(PlaceHolderPID, QString::number(message->pid));
    text.replace(PlaceHolderThreadId, QStringLiteral("%1").arg(quintptr(message->threadid), sizeof(quintptr)*2, 16, QLatin1Char('0')));
    text.replace(PlaceHolderThreadPtr, QStringLiteral("%1").arg(quintptr(message->threadptr), sizeof(quintptr)*2, 16, QLatin1Char('0')));
    text.replace(PlaceHolderFile, message->file);
    text.replace(PlaceHolderLine, QString::number(message->line));
    text.replace(PlaceHolderFunction, message->function);
    text.replace(PlaceHolderMessage, message->message);
    text += '\x1e';
    return text;
}

inline Log::LogLevel AbstractLogProcessor::filter() const
{
    return f;
}

inline void AbstractLogProcessor::setFilter(LogLevel logLevel)
{
    f = logLevel;
    emit filterChanged(f);
}

inline AbstractLogProcessor::categoryFilter_t AbstractLogProcessor::categoryFilters() const
{
    return cf;
}

inline void AbstractLogProcessor::setCategoryFilters(const categoryFilter_t& filters)
{
    QSet<QString> categories = cf.keys().toSet();
    categories += filters.keys().toSet();

    cf.clear();
    for (auto it = filters.cbegin(); it != filters.cend(); ++it)
    {
        if (it.value() != f) {
            cf[it.key()] = it.value();
        }
    }

    emit categoryFiltersChanged(cf);
    for (const QString& category : categories)
    {
        emit categoryFilterChanged(category, categoryFilter(category));
    }
}

inline Log::LogLevel AbstractLogProcessor::categoryFilter(const QString& category) const
{
    return cf.value(LogMessage::identifyDefaultCategory(category), f);
}

inline void AbstractLogProcessor::setCategoryFilter(QString category, LogLevel logLevel)
{
    category = LogMessage::identifyDefaultCategory(category);
    if (logLevel == f) {
        cf.remove(category);
    } else {
        cf[category] = logLevel;
    }
    emit categoryFilterChanged(category, logLevel);
    emit categoryFiltersChanged(cf);
}

inline void AbstractLogProcessor::newLog(const QSharedPointer<LogMessage>& message)
{
    if (LogLevel::filterAcceptLogLevel(categoryFilter(message->category),
                                       message->level))
    {
        process(message);
    }
}
} // namespace Impl
} // namespace Log

#endif // ABSTRACTLOGPROCESSOR_H
