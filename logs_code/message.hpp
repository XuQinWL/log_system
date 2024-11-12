#pragma once

#include <memory>
#include <thread>

#include "level.hpp"
#include "util.hpp"

namespace xqlog {
struct LogMessage {
    using ptr = std::shared_ptr<LogMessage>;
    LogMessage() = default;
    LogMessage(LogLevel::value level, std::string file, size_t line,
               std::string name, std::string payload)
        : name_(name),
          file_name_(file),
          payload_(payload),
          level_(level),
          line_(line),
          ctime_(Util::Date::Now()),
          tid_(std::this_thread::get_id()) {}

    size_t line_;            // 行号
    time_t ctime_;           // 时间
    std::string file_name_;  // 文件名
    std::string name_;       // 日志器名
    std::string payload_;    // 信息体
    std::thread::id tid_;    // 线程id
    LogLevel::value level_;  // 等级
};
}  // namespace xqlog