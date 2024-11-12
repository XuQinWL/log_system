#pragma once
#include "logger.hpp"
namespace xqlog {
// 用户获取日志器
Logger::ptr GetLogger(const std::string &name) {
    return LoggerManager::GetInstance().GetLogger(name);
}
// 用户获取默认日志器
Logger::ptr RootLogger() { return LoggerManager::GetInstance().RootLogger(); }

// 简化用户使用，宏函数默认填上文件吗+行号
#define Debug(fmt, ...) Debug(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Info(fmt, ...) Info(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Warn(fmt, ...) Warn(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Error(fmt, ...) Error(__FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define Fatal(fmt, ...) Fatal(__FILE__, __LINE__, fmt, ##__VA_ARGS__)

// 无需获取日志器，默认标准输出
#define LOGDEBUGDEFAULT(fmt, ...) xqlog::RootLogger()->Debug(fmt, ##__VA_ARGS__)
#define LOGINFODEFAULT(fmt, ...) xqlog::RootLogger()->Info(fmt, ##__VA_ARGS__)
#define LOGWARNDEFAULT(fmt, ...) xqlog::RootLogger()->Warn(fmt, ##__VA_ARGS__)
#define LOGERRORDEFAULT(fmt, ...) xqlog::RootLogger()->Error(fmt, ##__VA_ARGS__)
#define LOGFATALDEFAULT(fmt, ...) xqlog::RootLogger()->Fatal(fmt, ##__VA_ARGS__)
}  // namespace xqlog