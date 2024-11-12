/*
日志落地模块
确定日志最后的实现方式
1.标准输出
2.文件
3.滚动文件
*/
#include <cassert>
#include <fstream>
#include <memory>

#include "util.hpp"

namespace xqlog {

class LogSink {
   public:
    using ptr = std::shared_ptr<LogSink>;
    virtual ~LogSink() {}
    virtual void Sink(const char* data, size_t len) = 0;  // 用于落地具体方法
};

class StdoutSink : public LogSink {
   public:
    using ptr = std::shared_ptr<StdoutSink>;
    void Sink(const char* data, size_t len) override { cout.write(data, len); }
};
class FileSink : public LogSink {
   public:
    using ptr = std::shared_ptr<FileSink>;
    FileSink(const std::string& filename) : filename_(filename) {
        // 创建所给目录
        Util::File::CreateDirectory(Util::File::Path(filename));
        // 打开文件
        ofs_.open(filename.c_str(), std::ios::binary | std::ios::app);
        assert(ofs_.is_open());
    }
    void Sink(const char* data, size_t len) override {
        // 对文件进行写入
        assert(ofs_.good());
        ofs_.write(data, len);
    }

   private:
    std::string filename_;
    std::ofstream ofs_;
};

class RollFileSink : public LogSink {
   public:
    using ptr = std::shared_ptr<RollFileSink>;
    RollFileSink(const std::string& filename, size_t max_size)
        : max_size_(max_size), basename_(filename) {
        Util::File::CreateDirectory(Util::File::Path(filename));
    }

    void Sink(const char* data, size_t len) override {
        // 确认文件大小不满足滚动需求，并确保文件已经被打开且与文件流关联
        InitLogFile();
        // 向文件写入内容
        ofs_.good();
        ofs_.write(data, len);
        cur_size_ += len;
    }

   private:
    void InitLogFile() {
        if (ofs_.is_open() == false || cur_size_ >= max_size_) {
            ofs_.close();  // 就算文件没有打开使用close也是安全的
            std::string filename = CreateFilename();
            ofs_.open(filename, std::ios::binary | std::ios::app);
            assert(ofs_.is_open());
            cur_size_ = 0;
        }
    }

    // 构建落地的滚动日志文件名称
    std::string CreateFilename() {
        time_t time_ = Util::Date::Now();
        struct tm t;
        localtime_r(&time_, &t);
        std::string filename = basename_;
        filename += std::to_string(t.tm_year + 1900);
        filename += std::to_string(t.tm_mon + 1);
        filename += std::to_string(t.tm_mday);
        filename += std::to_string(t.tm_hour + 1);
        filename += std::to_string(t.tm_min + 1);
        filename += std::to_string(t.tm_sec + 1) + '-' +
                    std::to_string(cnt_++) + ".log";
        return filename;
    }

   private:
    size_t cnt_ = 1;
    size_t cur_size_ = 0;
    size_t max_size_;
    std::string basename_;
    std::ofstream ofs_;
};

class LogSinkFactory {
   public:
    using ptr = std::shared_ptr<LogSinkFactory>;
    template <typename SinkType, typename... Args>
    static std::shared_ptr<LogSink> CreateLog(Args&&... args) {
        return std::make_shared<SinkType>(std::forward<Args>(args)...);
    }
};
}  // namespace xqlog