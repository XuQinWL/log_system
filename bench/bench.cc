/*性能测试*/
#include "../logs_code/xqlog.hpp"
void bench(const std::string& logger_name, size_t thread_count,
           size_t msg_count, size_t msg_len) {
    // 1．获取日志器
    xqlog::Logger::ptr logger = xqlog::GetLogger(logger_name);
    if (logger.get() == nullptr) return;
    // 2．组织指定长度的日志消息
    std::string msg(msg_len - 1, 'X');
    // 3．创建指定数量的线程
    std::vector<std::thread> threads;
    std::vector<double> cost_array(thread_count, 0);
    size_t meg_per_thr = msg_count / thread_count;  // 每个线程所输出的日志条数
    for (int i = 0; i < thread_count; i++) {
        threads.emplace_back([&, i]() {
            // 4．线程函数内部开始计时
            auto start = std::chrono::high_resolution_clock::now();
            // 5．开始循环写日志
            for (int j = 0; j < meg_per_thr; j++) {
                logger->Error("%s", msg.c_str());
            }
            // 6．线程函数内部结束计时
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> cost = end - start;
            cost_array[i] = cost.count();
            std::cout << "线程" << i << ": "
                      << "\t输出数量——" << meg_per_thr << ", 总耗时："
                      << cost.count() << "s" << std::endl;
        });
        threads[i].join();
    }

    // 7．计算总耗时
    double max_cost = 0;
    for (auto cost : cost_array) max_cost = max_cost < cost ? cost : max_cost;
    size_t msg_per_sec = msg_count / max_cost;
    size_t size_per_sec = (msg_count * msg_len) / (max_cost * 1024);
    // 8．进行输出打印
    std::cout << "每秒输出日志数量:" << msg_per_sec << "条\n";
    std::cout << "每秒输出日志大小: " << size_per_sec << "KB\n";
}
void sync_bench() {
    std::shared_ptr<xqlog::LoggerBuilder> Glb(new xqlog::GlobalLoggerBuilder());
    Glb->BuildLoggerFormatter("%m%n");
    Glb->BuildLoggerName("synclogger");
    Glb->BuildLoggerType(xqlog::LoggerType::SYNCLOGGER);
    Glb->BuildLoggerSink<xqlog::FileSink>("./logfile/logfile.log");
    Glb->Build();
    bench("synclogger", 3, 1000000, 100);
}
void async_bench() {
    std::shared_ptr<xqlog::LoggerBuilder> Glb(new xqlog::GlobalLoggerBuilder());
    Glb->BuildLoggerFormatter("%m%n");
    Glb->BuildLoggerName("asynclogger");
    Glb->BuildLoggerType(xqlog::LoggerType::ASYNCLOGGER);
    Glb->BuildLoggerSink<xqlog::FileSink>("./logfile/Async_logfile.log");
    Glb->Build();
    bench("asynclogger",3, 1000000, 100);
}

int main() {
    // sync_bench();
    async_bench();
    return 0;
}