#ifndef LOGFILESAVERBASE_H
#define LOGFILESAVERBASE_H

#include <QtCore/QtCore>
#include "../Log.h"

#ifdef LOGFILESAVER_LIBRARY
# define LOGFILESAVER_SHARED_EXPORT Q_DECL_EXPORT
#elif defined(LOGFILESAVER_USE_CODE)
# define LOGFILESAVER_SHARED_EXPORT
#else
# define LOGFILESAVER_SHARED_EXPORT Q_DECL_IMPORT
#endif

namespace Log {
namespace Impl {
class LogFileSaveTask;
class LogFileSaverBasePrivate;

// base class for save log messages into file
class LOGFILESAVER_SHARED_EXPORT LogFileSaverBase : public QObject, public AbstractLogProcessor
{
    Q_OBJECT
    Q_PROPERTY(quint8 filter READ filter WRITE setFilter NOTIFY filterChanged)
    Q_PROPERTY(categoryFilter_t categoryFilters READ categoryFilters WRITE setCategoryFilters NOTIFY categoryFiltersChanged)

    /* ======== All properties will be set to all tasks, replacing their own value ======== */
    // Absolute path. Default is ./Log
    Q_PROPERTY(QString dir READ dir WRITE setDir)
    // Maximum size for single log file. Default 1 GB
    Q_PROPERTY(qint64 maxFileSize READ maxFileSize WRITE setMaxFileSize)
    // Log format. Default is Log::Impl::DefaultPattern
    Q_PROPERTY(QString format READ format WRITE setFormat)

public:
    explicit LogFileSaverBase(Qt::ConnectionType connectionType, QObject* parent = 0);
    virtual ~LogFileSaverBase();

    // AbstractLogProcessor interface
    Q_SIGNAL virtual void filterChanged(LogLevel logLevel) override;
    Q_SIGNAL virtual void categoryFiltersChanged(const categoryFilter_t& filters) override;
    Q_SIGNAL virtual void categoryFilterChanged(const QString& category, LogLevel logLevel) override;

    QString dir() const;
    void setDir(const QString& path);

    qint64 maxFileSize() const;
    void setMaxFileSize(qint64 value);

    QString format() const;
    void setFormat(const QString& value);

protected:
    // AbstractLogProcessor interface
    virtual void process(const QSharedPointer<LogMessage>& message) final;
    virtual void onFilterChanged(LogLevel logLevel) final;

    // Create save task for input category
    virtual LogFileSaveTask* createTask(const QString& category) const = 0;

private:
    Q_DECLARE_PRIVATE(LogFileSaverBase)
    LogFileSaverBasePrivate* d_ptr;
};


inline qint64 parseSize(QString size)
{
    qint64 base = 0;
    qint64 index = 0;

    size = size.toLower();
    if(size.endsWith(QStringLiteral("kb")))
        index = 10;
    else if(size.endsWith(QStringLiteral("mb")))
        index = 20;
    else if(size.endsWith(QStringLiteral("gb")))
        index = 30;
    else if(size.endsWith(QStringLiteral("tb")))
        index = 40;
    else if(size.endsWith(QStringLiteral("pb")))
        index = 50;
    else if(size.endsWith(QStringLiteral("eb")))
        index = 60;

    if(index != 0)
        size.chop(2);
    base = size.toLongLong();

    return qint64(base * std::pow(2, index));
}
} // namespace Log
} // namespace Impl

#endif // LOGFILESAVERBASE_H
