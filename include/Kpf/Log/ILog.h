#ifndef ILOG_H
#define ILOG_H

#include "Log.h"
#include "ILogCommon.h"
#include <Service.h>

// 放置在需要使用日志的类声明中，或全局函数内容开头
// 若无此宏，则编译时会在记录日志的位置报错，错误内容为“_logger未定义”
#define ENABLE_LOG \
Fw::ObjectRef<Log::Impl::ILogEngine> _logger = Fw::Service::getServiceRef(QStringLiteral("generic.Log.LogEngine"));

/* 获取日志输出流，具备category分类
 * category使用字符串标识，可以配置等级过滤
 * 过滤掉不处理的category不会产生任何性能开销
 */
#define fwCLog(category, logLevel, ...) \
for (Log::Impl::ILogEngine* engine = Log::Impl::getEngine(_logger, category, logLevel); engine; engine = nullptr) \
    engine->getStream(category, logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
#define fwCDebug(category, ...) fwCLog(category, Log::Debug, ##__VA_ARGS__)
#define fwCInformation(category, ...) fwCLog(category, Log::Information, ##__VA_ARGS__)
#define fwCWarning(category, ...) fwCLog(category, Log::Warning, ##__VA_ARGS__)
#define fwCCritical(category, ...) fwCLog(category, Log::Critical, ##__VA_ARGS__)
#define fwCFatal(category, ...) fwCLog(category, Log::Fatal, ##__VA_ARGS__)

/* 获取日志输出流，语法与qDebug()完全相同
 * C风格：
 *     fwDebug("0x%04X", 0xabcd);
 * C++风格：
 *     fwDebug() << "Hello World";
 */
#define fwLog(logLevel, ...) fwCLog(QString(), logLevel, ##__VA_ARGS__)
#define fwDebug(...) fwLog(Log::Debug, ##__VA_ARGS__)
#define fwInformation(...) fwLog(Log::Information, ##__VA_ARGS__)
#define fwWarning(...) fwLog(Log::Warning, ##__VA_ARGS__)
#define fwCritical(...) fwLog(Log::Critical, ##__VA_ARGS__)
#define fwFatal(...) fwLog(Log::Fatal, ##__VA_ARGS__)

namespace Log {
namespace Impl {
inline ILogEngine* getEngine(Fw::ObjectRef<Log::Impl::ILogEngine>& logger, const QString& category, Log::LogLevel logLevel)
{
    auto engine = &*logger;
    if (engine) {
        if (engine->filterAcceptLogLevel(category, logLevel)) {
            return engine;
        }
    }
    return nullptr;
}
} // namespace Impl
} // namespace Log

#endif // ILOG_H
