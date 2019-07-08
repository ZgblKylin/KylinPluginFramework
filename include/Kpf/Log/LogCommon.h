#ifndef LOGCOMMON_H
#define LOGCOMMON_H

#include <QtCore/QtCore>

namespace Log {
// 日志级别，接受任意整数，下为内置级别
struct LogLevel
{
    LogLevel();
    LogLevel(quint8 value);

    quint8 value() const;

    // 根据日志级别（0-100）获取对应的日志类型
    LogLevel getLogType() const;
    QString toString() const;

    // 判断输入日志等级是否应保留，大于等于过滤级别的日志将被保留
    static bool filterAcceptLogLevel(LogLevel filter, LogLevel logLevel);

    operator quint8() const;
    bool operator==(LogLevel other);
    bool operator!=(LogLevel other);
    bool operator<(LogLevel other);
    bool operator<=(LogLevel other);
    bool operator>(LogLevel other);
    bool operator>=(LogLevel other);

private:
    quint8 v;
};

static const quint8 Debug = 0;        // 0-24为 调试 级别
static const quint8 Information = 25; // 25-49为 信息 级别
static const quint8 Warning = 50;     // 50-74为 警告 级别
static const quint8 Critical = 75;    // 75-99为 危险 级别
static const quint8 Fatal = 100;      // 100为 致命 级别

inline LogLevel::LogLevel()
{
    v = Information;
}

inline LogLevel::LogLevel(quint8 value)
    : v(std::min(quint8(100), value))
{
}

inline quint8 LogLevel::value() const
{
    return *this;
}

inline LogLevel LogLevel::getLogType() const
{
    static const LogLevel LogTypeTable[101] = {
        // 0 - 24 is Debug
        Debug, Debug, Debug, Debug, Debug,
        Debug, Debug, Debug, Debug, Debug,
        Debug, Debug, Debug, Debug, Debug,
        Debug, Debug, Debug, Debug, Debug,
        Debug, Debug, Debug, Debug, Debug,

        // 25 - 49 is Information
        Information, Information, Information, Information, Information,
        Information, Information, Information, Information, Information,
        Information, Information, Information, Information, Information,
        Information, Information, Information, Information, Information,
        Information, Information, Information, Information, Information,

        // 50 - 74 is Warning
        Warning, Warning, Warning, Warning, Warning,
        Warning, Warning, Warning, Warning, Warning,
        Warning, Warning, Warning, Warning, Warning,
        Warning, Warning, Warning, Warning, Warning,
        Warning, Warning, Warning, Warning, Warning,

        // 75 - 99 is Critical
        Critical, Critical, Critical, Critical, Critical,
        Critical, Critical, Critical, Critical, Critical,
        Critical, Critical, Critical, Critical, Critical,
        Critical, Critical, Critical, Critical, Critical,
        Critical, Critical, Critical, Critical, Critical,

        // 100 is Fatal
        Fatal
    };
    return LogTypeTable[value()];
}

inline QString LogLevel::toString() const
{
    static const QString LogTypeStringForEnglish[5] = {
        QStringLiteral("Debug"),
        QStringLiteral("Information"),
        QStringLiteral("Warning"),
        QStringLiteral("Critical"),
        QStringLiteral("Fatal")
    };
    static const QString LogTypeStringForChinese[5] = {
        QStringLiteral("调试"),
        QStringLiteral("信息"),
        QStringLiteral("警告"),
        QStringLiteral("严重"),
        QStringLiteral("致命")
    };

    LogLevel type = getLogType();
    if (QLocale().language() == QLocale::Chinese) {
        return LogTypeStringForChinese[type / 25];
    } else {
        return LogTypeStringForEnglish[type / 25];
    }
}

inline bool LogLevel::filterAcceptLogLevel(LogLevel filter, LogLevel logLevel)
{
    return logLevel >= filter;
}

inline LogLevel::operator quint8() const
{
    return v;
}

inline bool LogLevel::operator==(LogLevel other)
{
    quint8 a = *this;
    quint8 b = other;
    return a == b;
}

inline bool LogLevel::operator!=(LogLevel other)
{
    return !(*this == other);
}

inline bool LogLevel::operator<(LogLevel other)
{
    quint8 a = *this;
    quint8 b = other;
    return a < b;
}

inline bool LogLevel::operator<=(LogLevel other)
{
    return (*this < other) || (*this == other);
}

inline bool LogLevel::operator>(LogLevel other)
{
    return !(*this <= other);
}

inline bool LogLevel::operator>=(LogLevel other)
{
    return !(*this < other);
}
} // namespace Log
Q_DECLARE_METATYPE(Log::LogLevel)

#endif // LOGCOMMON_H
