#include "../logs_code/xqlog.hpp"

enum class TimeGap { GAP_SECOND, GAP_MINUTE, GAP_HOUR, GAP_DAY };

// 日志器落地扩展---以时间为单位进行划分文件
class RollByTimeSink : public xqlog::LogSink {
   public:
    using ptr = std::shared_ptr<RollByTimeSink>;
    RollByTimeSink(const std::string &basename, TimeGap gap_type)
        : basename_(basename) {
        // 获取滚动的时间间隔
        switch (gap_type) {
            case TimeGap::GAP_SECOND:
                gap_size_ = 1;
                break;
            case TimeGap::GAP_MINUTE:
                gap_size_ = 60;
                break;
            case TimeGap::GAP_HOUR:
                gap_size_ = 360;
                break;
            case TimeGap::GAP_DAY:
                gap_size_ = 3600 * 24;
                break;
        }
        // 获取当前文件创建时间，创建文件所在目录并打开文件
        file_open_time_ = xqlog::Util::Date::Now();
        std::string filename = CreateNewFile();
        xqlog::Util::File::CreateDirectory(xqlog::Util::File::Path(filename));
        ofs_.open(filename, std::ios::binary | std::ios::app);
        assert(ofs_.is_open());
    }
    // 将日志消息写入到标准输出,判断当前时间是否是当前文件的时间段，不是则切换文件
    void Sink(const char *data, size_t len) {
        time_t cur = xqlog::Util::Date::Now();
        if ((cur - file_open_time_) / gap_size_ >= 1) {
            ofs_.close();
            std::string filename = CreateNewFile();
            file_open_time_ = xqlog::Util::Date::Now();  // 更新文件开启时间
            ofs_.open(filename, std::ios::binary | std::ios::app);
            assert(ofs_.is_open());
        }
        assert(ofs_.good());
        ofs_.write(data, len);
    }

   private:
    std::string CreateNewFile() {
        time_t t = xqlog::Util::Date::Now();
        struct tm lt;
        localtime_r(&t, &lt);
        std::stringstream filename;
        filename << basename_;
        filename << lt.tm_year + 1900;
        filename << lt.tm_mon + 1;
        filename << lt.tm_mday;
        filename << lt.tm_hour;
        filename << lt.tm_min;
        filename << lt.tm_sec;
        filename << ".log";
        return filename.str();
    }

   private:
    std::string basename_;
    std::ofstream ofs_;
    size_t file_open_time_;  // 滚动文件打开的时间
    size_t gap_size_;        // 滚动文件时间间隔
};

void test() {
    int cnt = 0;
    time_t old = time(nullptr);
    while (time(nullptr) < old + 5) {
        xqlog::GetLogger("asynclogger")->Fatal("测试日志-%d", ++cnt);
    }
}
int main() {
    std::shared_ptr<xqlog::LoggerBuilder> Glb(new xqlog::GlobalLoggerBuilder());
    Glb->BuildLoggerFormatter("[%c][%p][%f-%l]%T%m%n");
    Glb->BuildLoggerName("asynclogger");
    Glb->BuildLoggerType(xqlog::LoggerType::ASYNCLOGGER);
    Glb->BuildLoggerSink<RollByTimeSink>("./logfile/roll_bytime_logfile.log",
                                         TimeGap::GAP_SECOND);
    Glb->Build();
    test();

    return 0;
}