#ifndef LOGSTREAM_H
#define LOGSTREAM_H

#include <functional>
#include <cstdarg>
#include "LogCommon.h"

namespace Log {
namespace Impl {
#pragma pack(push, 1)
/**
 * @brief 日志条目数据结构
 */
struct LogMessage
{
    QString category;       // 日志分类
    QDateTime dateTime;     // 日志生成时间
    LogLevel level;         // 日志级别
    qint64 pid;             // 日志所属进程
    Qt::HANDLE threadid;    // 日志生成线程
    QThread* threadptr;     // 日志生成线程
    QString file;           // 文件名
    int line;               // 行号
    QString function;       // 函数名
    QString message;        // 日志信息

    LogMessage() = default;
    LogMessage(const QString& category, LogLevel level, const QString& file, int line, const QString& function);

    static constexpr const char* DefaultCategory = "Default";
    static QString identifyDefaultCategory(const QString& category);
};
#pragma pack(pop)

/**
 * @brief 日志输出流接口，继承自QDebug
 */
struct LogStream : public QDebug
{
    LogStream(QSharedPointer<LogMessage> msg, std::function<void(QSharedPointer<LogMessage>)> onDel);
    ~LogStream();

    // C++流风格日志操作接口，等同于qDebug() <<
    LogStream& log();

    // C printf风格日志操作接口，等同于qDebug(format, ...)
    void log(const char* format, ...);
private:
    QSharedPointer<LogMessage> message; // 日志条目
    std::function<void(QSharedPointer<LogMessage>)> onDelete; // 输出流析构时调用函数，用于汇总信息，记录日志
};

inline LogMessage::LogMessage(const QString& cg, LogLevel lvl, const QString& fl, int ln, const QString& fn)
    : category(cg),
      dateTime(QDateTime::currentDateTime()),
      level(lvl),
      pid(qApp->applicationPid()),
      threadid(QThread::currentThreadId()),
      threadptr(QThread::currentThread()),
      file(fl),
      line(ln),
      function(fn)
{
}

inline QString LogMessage::identifyDefaultCategory(const QString& category)
{
    if (category.isEmpty()) {
        return DefaultCategory;
    } else {
        return category;
    }
}

inline LogStream::LogStream(QSharedPointer<LogMessage> msg, std::function<void(QSharedPointer<LogMessage>)> onDel)
    : QDebug(&(msg->message)),
      message(msg),
      onDelete(onDel)
{
}

inline LogStream::~LogStream()
{
    onDelete(std::move(message));
}

inline LogStream& LogStream::log()
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
this->noquote();
#endif
    return *this;
}

inline void LogStream::log(const char* format, ...)
{
    std::va_list args;
    va_start(args, format);
    int size = std::vsnprintf(nullptr, 0, format, args);
    QByteArray buf(size + 1, '\0'); // note +1 for null terminator
    std::vsnprintf(buf.data(), size_t(buf.size()), format, args);
    va_end(args);
    *this << buf.data();
}

// 转发所有输出
template<typename T>
LogStream& operator<<(LogStream& stream, const T& val)
{
    dynamic_cast<QDebug&>(stream) << val;
    return stream;
}

// 重载QString输出，删除引号
inline LogStream& operator<<(LogStream& stream, const QString& val)
{
    stream << val.toUtf8().constData();
    return stream;
}

// 重载QLatin1String输出，删除引号
inline LogStream& operator<<(LogStream& stream, const QLatin1String& val)
{
    stream << val.data();
    return stream;
}

// 重载QByteArray输出，删除引号
inline LogStream& operator<<(LogStream& stream, const QByteArray& val)
{
    stream << val.constData();
    return stream;
}
} // namespace Impl
} // namespace Log
Q_DECLARE_METATYPE(QSharedPointer<Log::Impl::LogMessage>)

#endif // LOGSTREAM_H
