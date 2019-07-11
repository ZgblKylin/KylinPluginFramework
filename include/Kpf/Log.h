#pragma once
#include <Kpf/Common.h>
#include <Kpf/Log/Log.h>

/* 获取日志输出流，具备category分类
 * category使用字符串标识，可以配置等级过滤
 * 过滤掉不处理的category不会产生任何性能开销
 */
#define kpfCLog(category, logLevel, ...) \
for (bool accepted = Kpf::getLogEngine()->filterAcceptLogLevel(category, logLevel); \
     accepted; accepted = false) \
    Kpf::getLogEngine()->getStream(category, logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
#define kpfCDebug(category, ...) kpfCLog(category, Log::Debug, ##__VA_ARGS__)
#define kpfCInformation(category, ...) kpfCLog(category, Log::Information, ##__VA_ARGS__)
#define kpfCWarning(category, ...) kpfCLog(category, Log::Warning, ##__VA_ARGS__)
#define kpfCCritical(category, ...) kpfCLog(category, Log::Critical, ##__VA_ARGS__)
#define kpfCFatal(category, ...) kpfCLog(category, Log::Fatal, ##__VA_ARGS__)

/* 获取日志输出流，语法与qDebug()完全相同
 * C风格：
 *     kpfDebug("0x%04X", 0xabcd);
 * C++风格：
 *     kpfDebug() << "Hello World";
 */
#define kpfLog(logLevel, ...) kpfCLog(QString(), logLevel, ##__VA_ARGS__)
#define kpfDebug(...) kpfLog(Log::Debug, ##__VA_ARGS__)
#define kpfInformation(...) kpfLog(Log::Information, ##__VA_ARGS__)
#define kpfWarning(...) kpfLog(Log::Warning, ##__VA_ARGS__)
#define kpfCritical(...) kpfLog(Log::Critical, ##__VA_ARGS__)
#define kpfFatal(...) kpfLog(Log::Fatal, ##__VA_ARGS__)

namespace Kpf {
KPFSHARED_EXPORT Log::Impl::ILogEngine* getLogEngine();
} // namespace Kpf
