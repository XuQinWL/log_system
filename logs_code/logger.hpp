/*
日志器模块
*/
#pragma once
#include <atomic>
#include <cassert>
#include <cstdarg>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "formatter.hpp"
#include "level.hpp"
#include "looper.hpp"
#include "message.hpp"
#include "sink.hpp"
namespace xqlog {
class Logger {
   public:
    using ptr = std::shared_ptr<Logger>;
    Logger(const std::string& logger_name, xqlog::LogLevel::value level,
           Formatter::ptr& fmter, std::vector<LogSink::ptr>& sinks)
        : logger_name_(logger_name),
          limit_level_(level),
          formatter_(fmter),
          sinks_(sinks.begin(), sinks.end()) {}
    virtual ~Logger(){};
    std::string Name() { return logger_name_; }
    void Debug(const std::string& file, size_t line, const std::string format,
               ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::DEBUG < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // 将va指针置空

        serialize(LogLevel::value::DEBUG, file, line,
                  ret);  // 3.生成格式化日志信息并落地

        free(ret);
        ret = nullptr;
    };
    void Info(const std::string& file, size_t line, const std::string format,
              ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::INFO < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // 将va指针置空

        serialize(LogLevel::value::INFO, file, line,
                  ret);  // 3.生成格式化日志信息并落地

        free(ret);
        ret = nullptr;
    };

    void Warn(const std::string& file, size_t line, const std::string format,
              ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::WARN < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // 将va指针置空

        serialize(LogLevel::value::WARN, file, line,
                  ret);  // 3.生成格式化日志信息并落地

        free(ret);
        ret = nullptr;
    };
    void Error(const std::string& file, size_t line, const std::string format,
               ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::ERROR < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // 将va指针置空

        serialize(LogLevel::value::ERROR, file, line,
                  ret);  // 3.生成格式化日志信息并落地

        free(ret);
        ret = nullptr;
    };
    void Fatal(const std::string& file, size_t line, const std::string format,
               ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::FATAL < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // 将va指针置空

        serialize(LogLevel::value::FATAL, file, line,
                  ret);  // 3.生成格式化日志信息并落地

        free(ret);
        ret = nullptr;
    };
    void Off(const std::string& file, size_t line, const std::string format,
             ...) {  // 用户所给格式用可变参数包，后面提取出格式
        // 1.判断日志等级是否需要输出到指定方向
        if (LogLevel::value::OFF < limit_level_) return;
        // 2.组织格式化格式-获取可变参数列表中的格式
        va_list va;
        va_start(va, format);
        char* ret;
        int r = vasprintf(&ret, format.c_str(), va);
        if (r == -1) perror("vasprintf failed!!!: ");
        va_end(va);  // va->NULL

        serialize(LogLevel::value::OFF, file, line,
                  ret);  // 3.生成格式化日志信息并落地到具体方向

        free(ret);
        ret = nullptr;
    };
    virtual void Sink(const char* data, size_t len) = 0;

   protected:
    void serialize(LogLevel::value level, const std::string& file, size_t line,
                   char* ret) {
        // std::cout << "Debug:serialize begin\n";
        LogMessage msg(level, file, line, logger_name_, ret);
        std::string data = formatter_->format(msg);

        // std::cout << "Debug:serialize Sink\n";
        Sink(data.c_str(), data.size());
    }

   protected:
    std::mutex mtx_;
    std::atomic<LogLevel::value> limit_level_;  // 限制等级输出
    std::string logger_name_;                   // 组织格式化格式
    Formatter::ptr formatter_;         // 生成格式化的日志信息
    std::vector<LogSink::ptr> sinks_;  // 输出到指定方向\
    std::vector<LogSink> sink_;不能使用logsink作为元素类型，logsink是纯虚类，不能实例化
};
class SyncLogger : public Logger {
   public:
    using ptr = std::shared_ptr<SyncLogger>;
    SyncLogger(const std::string& logger_name, xqlog::LogLevel::value level,
               Formatter::ptr& fmter, std::vector<LogSink::ptr>& sinks)
        : Logger(logger_name, level, fmter, sinks) {
    }  // 父类成员变量由父类构造初始化

    void Sink(const char* data, size_t len) override {
        if (sinks_.empty()) return;
        for (auto& e : sinks_) {
            e->Sink(data, len);
        }
    }
};

class AsyncLogger : public Logger {
   public:
    using ptr = std::shared_ptr<AsyncLogger>;
    AsyncLogger(const std::string& logger_name, xqlog::LogLevel::value level,
                Formatter::ptr& fmter, std::vector<LogSink::ptr>& sinks,
                AsyncType type)
        : Logger(logger_name, level, fmter, sinks),
          looper_(std::make_shared<AsyncLooper>(
              std::bind(&AsyncLogger::RealSink, this, std::placeholders::_1),
              type)) {
        // std::cout << LogLevel::ToString(level) << "异步日志器: " <<
        // logger_name
        //           << "创建成功...\n";
    }

    // 主线程serialize调用该函数写入缓冲区便相当于完成一次落地，之后实际落地由异步线程完成
    void Sink(const char* data, size_t len) override {
        looper_->Push(data, len);  // Push函数本身是线程安全的，这里不加锁
    }

    void RealSink(Buffer& buffer) {  // 由异步线程进行实际落地
        if (sinks_.empty()) return;
        for (auto& e : sinks_) {
            e->Sink(buffer.Begin(), buffer.ReadableSize());
        }
    }

   private:
    xqlog::AsyncLooper::ptr looper_;
};

// 日志器建造者模式
enum class LoggerType { SYNCLOGGER, ASYNCLOGGER };
class LoggerBuilder {
   public:
    using ptr = std::shared_ptr<LoggerBuilder>;
    void BuildLoggerName(const std::string& name) { logger_name_ = name; }
    void BuildLoggerType(LoggerType type) { logger_type_ = type; }
    void BuildLopperType(AsyncType type) { looper_type_ = type; }
    void BuildLoggerLevel(LogLevel::value level) { limit_level_ = level; }
    void BuildLoggerFormatter(const std::string pattern) {
        formatter_ = std::make_shared<Formatter>(pattern);
    }

    template <typename SinkType, typename... Args>
    void BuildLoggerSink(Args&&... args) {
        sinks_.emplace_back(
            LogSinkFactory::CreateLog<SinkType>(std::forward<Args>(args)...));
    }
    virtual Logger::ptr Build() = 0;

   protected:
    std::string logger_name_ = "sync_logger";               // 日志器名称
    LoggerType logger_type_ = LoggerType::SYNCLOGGER;       // 日志器类型
    LogLevel::value limit_level_ = LogLevel::value::DEBUG;  // 传入限制等级
    Formatter::ptr formatter_;                              // 格式化器
    std::vector<xqlog::LogSink::ptr> sinks_;                // 落地方向
    AsyncType looper_type_ = AsyncType::ASYNC_SAFE;
};

// 局部建造者
class LocalLoggerBuilder : public LoggerBuilder {
   public:
    using ptr = std::shared_ptr<LocalLoggerBuilder>;
    Logger::ptr Build() override {
        if (formatter_.get() == nullptr)  // 保证格式化器被生成
            formatter_ = std::make_shared<Formatter>();
        if (sinks_.empty())  // 默认落地方向
            sinks_.emplace_back(std::make_shared<StdoutSink>());
        if (logger_type_ == LoggerType::SYNCLOGGER)  // 同步日志器
        {
            return std::make_shared<SyncLogger>(logger_name_, limit_level_,
                                                formatter_, sinks_);
        }
        // 异步日志器
        return std::make_shared<AsyncLogger>(logger_name_, limit_level_,
                                             formatter_, sinks_, looper_type_);
    }
};

// 通过单例对象对日志器的使用，懒汉模式
class LoggerManager {
   public:
    static LoggerManager& GetInstance() {
        // c++11之后，编译器在编译期间将静态变量进行处理，是线程安全的，不必加锁
        static LoggerManager eton;//eton对象在第一次调用 GetInstance() 方法时会被构造，但在后续调用中，\
        不会反复调用构造函数。
        return eton;
    }

    bool LoggerExist(const std::string& name) {
        std::unique_lock<std::mutex> lock(mtx_);
        auto it = loggers_.find(name);
        if (it == loggers_.end()) {
            return false;
        }
        return true;
    }

    void AddLogger(Logger::ptr& logger) {
        if (LoggerExist(logger->Name())) return;
        std::unique_lock<std::mutex> lock(mtx_);
        loggers_.insert(std::make_pair(logger->Name(), logger));
    }

    Logger::ptr GetLogger(const std::string& name) {
        std::unique_lock<std::mutex> lock(mtx_);
        auto it = loggers_.find(name);
        if (it == loggers_.end()) {
            return Logger::ptr();
        }
        return it->second;
    }

    Logger::ptr RootLogger() { return root_logger_; }

   private:
    LoggerManager() {
        std::unique_ptr<LoggerBuilder> builder(new LocalLoggerBuilder());//默认使用local日志器建造
        builder->BuildLoggerName("default");
        root_logger_ = builder->Build();
        loggers_.insert(std::make_pair("default", root_logger_));
    }

   private:
    std::mutex mtx_;
    Logger::ptr root_logger_;                               // 默认日志器
    std::unordered_map<std::string, Logger::ptr> loggers_;  // 存放日志器
};

// 全局建造者
class GlobalLoggerBuilder : public LoggerBuilder {
   public:
    Logger::ptr Build() override {
        assert(logger_name_.empty() == false);
        // 必须有日志器名称
        if (formatter_.get() == nullptr) {
            formatter_ = std::make_shared<Formatter>();
        }
        //如果落地方式没有指定，那么采用默认的标准输出
        if (sinks_.empty()) LogSinkFactory::CreateLog<xqlog::StdoutSink>();
        Logger::ptr logger;//通过建造者中传入的日志器类型选择使用同步还是异步
        //这里同步异步日志器都继承自logger，子类赋值父类-语法支持
        if (logger_type_ == LoggerType::ASYNCLOGGER) {
            logger = std::make_shared<AsyncLogger>(
                logger_name_, limit_level_, formatter_, sinks_, looper_type_);
        } else {
            logger = std::make_shared<SyncLogger>(logger_name_, limit_level_,
                                                  formatter_, sinks_);
        }
        // 建造者把日志器给管理对象，调用者通过调用单例管理对象对日志进行落地
        LoggerManager::GetInstance().AddLogger(logger);//获取单例会建造默认日志器，需要Addlogger
        return logger;
    }
};
}  // namespace xqlog