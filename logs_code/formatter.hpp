/*日志格式化模块*/
#pragma once

#include <cassert>
#include <ctime>
#include <sstream>
#include <vector>

#include "level.hpp"
#include "message.hpp"
#include "util.hpp"

namespace xqlog {
// 日志格式化子项类,用于组装一条完整的日志消息中的组件
class FormatItem {
   public:
    using ptr = std::shared_ptr<FormatItem>;
    virtual ~FormatItem() {
    }  // 基类析构函数定义虚函数，保证子类资源正确释放，否则只会调用基类的析构
    virtual void format(std::ostream &os, const LogMessage &msg) = 0;
};

class MsgFormatItem : public FormatItem {
   public:
    MsgFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << msg.payload_;
    }
};
class LevelFormatItem : public FormatItem {
   public:
    LevelFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << LogLevel::ToString(msg.level_);
    }
};
class NameFormatItem : public FormatItem {
   public:
    NameFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << msg.name_;
    }
};
class ThreadFormatItem : public FormatItem {
   public:
    ThreadFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << msg.tid_;
    }
};
class TimeFormatItem : public FormatItem {
   public:
    TimeFormatItem(const std::string &format = "%H:%M:%S") : format_(format) {
        if (format.empty()) format_ = "%H:%M:%S";
    }

    virtual void format(std::ostream &os, const LogMessage &msg) {
        struct tm t;
        localtime_r(&msg.ctime_, &t);
        char buf[128];
        strftime(buf, sizeof(buf), format_.c_str(), &t);
        os << buf;
    }

   private:
    std::string format_;
};
class CFileFormatItem : public FormatItem {
   public:
    CFileFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << msg.file_name_;
    }
};
class CLineFormatItem : public FormatItem {
   public:
    CLineFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) {
        os << msg.line_;
    }
};
class TabFormatItem : public FormatItem {
   public:
    TabFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) { os << "\t"; }
};
class NLineFormatItem : public FormatItem {
   public:
    NLineFormatItem(const std::string &str = "") {}
    virtual void format(std::ostream &os, const LogMessage &msg) { os << "\n"; }
};
class OtherFormatItem : public FormatItem {
   private:
    std::string str_;

   public:
    OtherFormatItem(const std::string &str = "") : str_(str) {}
    virtual void format(std::ostream &os, const LogMessage &msg) { os << str_; }
};

// 正式对日志进行格式化
class Formatter {
    /*  %d 日期 %T 缩进 %t 线程id %p 日志级别 %c 日志器名称 %f 文件名 %l 行号 %m 日志消息 %n 换行 */
    // 时间{年-月-日 时:分:秒}缩进 线程ID 缩进 [日志级别] 缩进 [日志名称] 缩进
    // 文件名:行号 缩进 消息换行
   public:
    using ptr = std::shared_ptr<Formatter>;
    Formatter(
        const std::string pattern = "[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n")
        : pattern_(pattern) {
        // 验证所给日志格式是否正确，能够成功解析
         // 调用解析函数，解析出最后落地的日志格式，通过子格式接口放入数组\
        最后调用数组中对应接口返回对应日志信息，拼接成最后的日志
        assert(ParsePattern());
    }
    // 用来调用对应的子格式接口，子格式接口传入流中的字符串就是最后日志中的一部分格式化字符串
    void format(std::ostream &os, LogMessage &msg) {
        for (auto &it : item_) {
            it->format(os, msg);
        }
    }
    // 提供接口对外用来格式化字符串
    std::string format(LogMessage &msg) {
        std::stringstream ss;
        format(ss, msg);
        return ss.str();
    }
    // 解析需要落地的日志格式
    bool ParsePattern() {
        size_t pos = 0;
        std::string key, val;
        std::vector<std::pair<std::string, std::string>>
            sub_fmt;  // 存分割开的子格式

        // 遍历开发人员给的格式，进行子格式的分割
        while (pos < pattern_.size()) {
            // abc%%[%%%d{%H}][%t][%T]
            if (pattern_[pos] != '%') {
                val += pattern_[pos++];
                continue;
            } else {
                if (pos + 1 < pattern_.size() && pattern_[pos + 1] == '%') {
                    val += pattern_[pos];
                    pos += 2;
                    continue;
                }
            }
            if (!val.empty()) sub_fmt.emplace_back(std::make_pair("", val));
            val.clear();
            //"[%d{%H:%M:%S}][%t][%p][%c][%f:%l] %m%n"
            // 此时pos+1处的pattern_一定是%后面的格式化字符
            ++pos;
            key = pattern_[pos];
            if (pos + 1 < pattern_.size() && pattern_[pos + 1] == '{') {
                pos += 2;
                while (pos < pattern_.size() && pattern_[pos] != '}') {
                    val += pattern_[pos++];
                }
                if (pattern_[pos] != '}') {
                    std::cout << "落地日志格式错误" << std::endl;
                    return false;
                }
            }
            sub_fmt.emplace_back(std::make_pair(key, val));
            key.clear();
            val.clear();
            ++pos;
        }

        // 创建子格式对象，填充_item，用于调用format()接口构建日志信息
        for (auto t : sub_fmt) {
            item_.emplace_back(
                CreateItem(std::get<0>(t), std::get<1>(t)));  // get模板
        }
        return true;
    }

   private:
    // 构建子格式化字符串，这里构建完了在format接口处拼接成完整格式化日志字符串
    FormatItem::ptr CreateItem(const std::string &key, const std::string &val) {
        if (key == "d") return std::make_shared<TimeFormatItem>(val);
        if (key == "t") return std::make_shared<ThreadFormatItem>();
        if (key == "c") return std::make_shared<NameFormatItem>();
        if (key == "p") return std::make_shared<LevelFormatItem>();
        if (key == "n") return std::make_shared<NLineFormatItem>();
        if (key == "f") return std::make_shared<CFileFormatItem>();
        if (key == "l") return std::make_shared<CLineFormatItem>();
        if (key == "T") return std::make_shared<TabFormatItem>();
        if (key == "m") return std::make_shared<MsgFormatItem>();
        if (key.empty()) return std::make_shared<OtherFormatItem>(val);
        std::cout << "没有对应的格式化字符串" << std::endl;
        assert(false);
        return FormatItem::ptr();
        // \ 智能指针不传入值则值的内容就是nullptr，get()得到的就是nullptr
    }

   private:
    std::string pattern_;
    std::vector<FormatItem::ptr> item_;
};

}  // namespace xqlog