/*日志缓冲区类设计*/
#pragma once
#include <cassert>
#include <string>
#include <vector>
#include "util.hpp"

namespace xqlog{
    class Buffer{
    public:
        Buffer() : write_pos_(0), read_pos_(0) {
            std::string content;
            xqlog::Util::File file;
            if(file.GetContent(&content,"../logs_code/config.conf")==false){
                perror("open config.conf failed");
            }
            Json::Value root;
            xqlog::Util::JsonUtil::UnSerialize(content, &root); // 反序列化，把内容转成jaon value格式

            buffer_size_g = root["buffer_size"].asInt();
            buffer_.resize(buffer_size_g);//缓冲区基础容量
            threshold_g = root["threshold"].asInt64();// 倍数扩容阈值
            linear_growth_g = root["linear_growth"].asInt();// 线性增长容量
            cout<<"buffer_size_g:"<<buffer_size_g<<endl<<"threshold_g:"<<threshold_g<<endl
                <<"linear_growth_g:"<<linear_growth_g<<endl;
        }

        void Push(const char *data, size_t len)
        {
            ToBeEnough(len); // 确保容量足够
            // 开始写入
            std::copy(data, data + len, &buffer_[write_pos_]);
            // buffer_.emplace_back(*data);
            write_pos_ += len;
        }
        char *ReadBegin(int len)
        {
            assert(len <= ReadableSize());
            return &buffer_[read_pos_];
        }
        bool IsEmpty() { return write_pos_ == read_pos_; }

        void Swap(Buffer &buf)
        {
            buffer_.swap(buf.buffer_);
            std::swap(read_pos_, buf.read_pos_);
            std::swap(write_pos_, buf.write_pos_);
        }
        size_t WriteableSize()
        { // 写空间剩余容量
            return buffer_.size() - write_pos_;
        }
        size_t ReadableSize()
        { // 读空间剩余容量
            return write_pos_ - read_pos_;
        }
        const char *Begin() { return &buffer_[read_pos_]; }
        void MoveWritePos(int len)
        {
            assert(len <= WriteableSize());
            write_pos_ += len;
        }
        void MoveReadPos(int len)
        {
            assert(len <= ReadableSize());
            read_pos_ += len;
        }
        void Reset()
        { // 重置缓冲区
            write_pos_ = 0;
            read_pos_ = 0;
        }

    protected:
        void ToBeEnough(size_t len)
        {
            int buffersize = buffer_.size();
            if (len >= WriteableSize())
            {
                if (buffer_.size() < threshold_g)
                {
                    buffer_.resize(2 * buffer_.size() + buffersize);
                }
                else
                {
                    buffer_.resize(linear_growth_g + buffersize);
                }
            }
        }

    protected:
        std::vector<char> buffer_; // 缓冲区,不是环形队列
        size_t write_pos_;         // 生产者此时的位置
        size_t read_pos_;          // 消费者此时的位置
        size_t buffer_size_g;      // 缓冲区基础容量
        size_t threshold_g;        // 倍数扩容阈值
        size_t linear_growth_g;    // 线性增长容量
    };
} // namespace xqlog