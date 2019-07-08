#ifndef LOG_H
#define LOG_H

#include "LogCommon.h"
#include "LogStream.h"
#include "AbstractLogProcessor.h"
#include "LogEngine.h"

/* 获取日志输出流，具备category分类
 * category使用字符串标识，可以配置等级过滤
 * 过滤掉不处理的category不会产生任何性能开销
 */
#define kyCLog(category, logLevel, ...) \
for (bool accepted = Log::Impl::LogEngine::instance().filterAcceptLogLevel(category, logLevel); \
     accepted; accepted = false) \
    Log::Impl::LogEngine::instance().getStream(category, logLevel, __FILE__, __LINE__, Q_FUNC_INFO).log(__VA_ARGS__)
#define kyCDebug(category, ...) kyCLog(category, Log::Debug, ##__VA_ARGS__)
#define kyCInformation(category, ...) kyCLog(category, Log::Information, ##__VA_ARGS__)
#define kyCWarning(category, ...) kyCLog(category, Log::Warning, ##__VA_ARGS__)
#define kyCCritical(category, ...) kyCLog(category, Log::Critical, ##__VA_ARGS__)
#define kyCFatal(category, ...) kyCLog(category, Log::Fatal, ##__VA_ARGS__)

/* 获取日志输出流，语法与qDebug()完全相同
 * C风格：
 *     kyDebug("0x%04X", 0xabcd);
 * C++风格：
 *     kyDebug() << "Hello World";
 */
#define kyLog(logLevel, ...) kyCLog(QString(), logLevel, ##__VA_ARGS__)
#define kyDebug(...) kyLog(Log::Debug, ##__VA_ARGS__)
#define kyInformation(...) kyLog(Log::Information, ##__VA_ARGS__)
#define kyWarning(...) kyLog(Log::Warning, ##__VA_ARGS__)
#define kyCritical(...) kyLog(Log::Critical, ##__VA_ARGS__)
#define kyFatal(...) kyLog(Log::Fatal, ##__VA_ARGS__)

#endif // LOG_H
