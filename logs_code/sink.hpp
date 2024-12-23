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
#include <unistd.h>
#include "util.hpp"

namespace xqlog
{
    class LogSink
    {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink(){
            std::string content;
            xqlog::Util::File file;
            if (file.GetContent(&content, "../logs_code/config.conf") == false){
                perror("open config.conf failed");
            }
            Json::Value root;
            xqlog::Util::JsonUtil::UnSerialize(content, &root); // 反序列化，把内容转成jaon value格式
            flush_log_ = root["flush_log"].asInt();
        }
        virtual ~LogSink() {}
        virtual void Sink(const char *data, size_t len) = 0; // 用于落地具体方法
    protected:
        size_t flush_log_;
    };

    class StdoutSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<StdoutSink>;
        void Sink(const char *data, size_t len) override{
            cout.write(data, len);
        }
    };
    class FileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<FileSink>;
        FileSink(const std::string &filename) : filename_(filename)
        {
            // 创建所给目录
            Util::File::CreateDirectory(Util::File::Path(filename));
            // 打开文件
            fs_ = fopen(filename.c_str(), "ab");
            if(fs_==NULL){
                std::cout <<__FILE__<<__LINE__<<"open log file failed"<< std::endl;
                perror(NULL);
            }
        }
        void Sink(const char *data, size_t len) override{
            fwrite(data,1,len,fs_);
            if(ferror(fs_)){
                std::cout <<__FILE__<<__LINE__<<"write log file failed"<< std::endl;
                perror(NULL);
            }
            if(flush_log_ == 1){
                if(fflush(fs_)==EOF){
                    std::cout <<__FILE__<<__LINE__<<"fflush file failed"<< std::endl;
                    perror(NULL);
                }
            }else if(flush_log_ == 2){
                fsync(fileno(fs_));
            }
        }

    private:
        std::string filename_;
        FILE* fs_ = NULL; 
    };

    class RollFileSink : public LogSink
    {
    public:
        using ptr = std::shared_ptr<RollFileSink>;
        RollFileSink(const std::string &filename, size_t max_size)
            : max_size_(max_size), basename_(filename)
        {
            Util::File::CreateDirectory(Util::File::Path(filename));
        }

        void Sink(const char *data, size_t len) override
        {
            // 确认文件大小不满足滚动需求
            InitLogFile();
            // 向文件写入内容
            fwrite(data, 1, len, fs_);
            if(ferror(fs_)){
                std::cout <<__FILE__<<__LINE__<<"write log file failed"<< std::endl;
                perror(NULL);
            }
            cur_size_ += len;
            if(flush_log_ == 1){
                if(fflush(fs_)){
                    std::cout <<__FILE__<<__LINE__<<"fflush file failed"<< std::endl;
                    perror(NULL);
                }
            }else if(flush_log_ == 2){
                fsync(fileno(fs_));
            }
        }

    private:
        void InitLogFile()
        {
            if (fs_==NULL || cur_size_ >= max_size_)
            {
                if(fs_!=NULL){
                    fclose(fs_);
                    fs_=NULL;
                }   
                std::string filename = CreateFilename();
                fs_=fopen(filename.c_str(), "ab");
                if(fs_==NULL){
                    std::cout <<__FILE__<<__LINE__<<"open file failed"<< std::endl;
                    perror(NULL);
                }
                cur_size_ = 0;
            }
        }

        // 构建落地的滚动日志文件名称
        std::string CreateFilename()
        {
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
        // std::ofstream ofs_;
        FILE* fs_ = NULL;
    };

    class LogSinkFactory
    {
    public:
        using ptr = std::shared_ptr<LogSinkFactory>;
        template <typename SinkType, typename... Args>
        static std::shared_ptr<LogSink> CreateLog(Args &&...args)
        {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };
} // namespace xqlog