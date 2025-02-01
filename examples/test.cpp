#include "../logs_code/xqlog.hpp"
#include "../logs_code/thread_poll.hpp"
#include "../logs_code/util.hpp"
using std::cout;
using std::endl;

ThreadPool* tp=nullptr;
xqlog::Util::JsonData* g_conf_data;
void test() {
    int cur_size = 1;
    int cnt = 5;
    while (cur_size++ < 10) {
        xqlog::GetLogger("synclogger")->Info("测试日志-%d", cnt++);
        xqlog::GetLogger("synclogger")->Warn("测试日志-%d", cnt++);
        xqlog::GetLogger("synclogger")->Debug("测试日志-%d", cnt++);
        xqlog::GetLogger("synclogger")->Error("测试日志-%d", cnt++);
        xqlog::GetLogger("synclogger")->Fatal("测试日志-%d", cnt++);
        // xqlog::GetLogger("asynclogger")->Info("测试日志-%d", cnt++);
        // xqlog::GetLogger("asynclogger")->Warn("测试日志-%d", cnt++);
        // xqlog::GetLogger("asynclogger")->Debug("测试日志-%d", cnt++);
        // xqlog::GetLogger("asynclogger")->Error("测试日志-%d", cnt++);
        // xqlog::GetLogger("asynclogger")->Fatal("测试日志-%d", cnt++);
    }
}

void init_thread_pool() {
    tp = new ThreadPool(g_conf_data->thread_count);
}
int main() {
    g_conf_data = xqlog::Util::JsonData::GetJsonData();
    init_thread_pool();
    std::shared_ptr<xqlog::LoggerBuilder> Glb(new xqlog::GlobalLoggerBuilder());
    Glb->BuildLoggerFormatter("[%c][%p][%f-%l]%T%m%n");
    // Glb->BuildLoggerName("asynclogger");
    // Glb->BuildLoggerType(xqlog::LoggerType::ASYNCLOGGER);
    Glb->BuildLoggerName("synclogger");
    Glb->BuildLoggerType(xqlog::LoggerType::SYNCLOGGER);
    Glb->BuildLoggerSink<xqlog::FileSink>("./logfile/FileSink.log");
    Glb->BuildLoggerSink<xqlog::RollFileSink>("./logfile/RollFile_log",
                                              1024 * 1024);
    Glb->Build();//建造完成后，日志器已经建造，有LoggerManger类成员管理诸多日志器
    test();
    // tp->shutdown();
    return 0;
}

// // // 日志器缓冲区测试
// std::ifstream ifs("./a.pdf", std::ios::binary);

// ifs.seekg(0, std::ios::end);  // 读写位置跳转到文件末尾
// size_t filesize = ifs.tellg();
// ifs.seekg(0, std::ios::beg);
// std::string body;
// body.resize(filesize);
// ifs.read(&body[0], filesize);
// assert(ifs.good());
// ifs.close();
// xqlog::Buffer buffer;

// for (int i = 0; i < body.size(); i++) {
//     buffer.Push(&body[i], 1);
// }

// std::ofstream ofs("./tmp.pdf", std::ios::binary);

// size_t size = buffer.ReadableSize();
// for (int i = 0; i < size; i++) {
//     ofs.write(buffer.ReadBegin(1), 1);
//     buffer.MoveReadPos(1);
// }
// ofs.close();
/*日志器建造者模式测试*/
// std::shared_ptr<xqlog::LocalLoggerBuilder> llb(
//     new xqlog::LocalLoggerBuilder());
// llb->BuildLoggerFormatter("[%c][%p][%f-%l]%T%m%n");
// llb->BuildLoggerName("asynclogger");
// llb->BuildLoggerType(xqlog::LoggerType::ASYNCLOGGER);
// llb->BuildLoggerSink<xqlog::FileSink>("./logfile.log");

// xqlog::Logger::ptr logger = llb->Build();
// logger->Debug(__FILE__, __LINE__, "测试日志-1");
// logger->Info(__FILE__, __LINE__, "测试日志-2");
// logger->Error(__FILE__, __LINE__, "测试日志-3");
// int cur_size =3;
// int cnt = 4;
// while (cur_size++ < 100000) {
//     logger->Fatal(__FILE__, __LINE__, "测试日志-%d", cnt++);
// }
// /*日志器模块测试*/
// std::string loggername = "sync_logger";
// xqlog::LogLevel::value limit = xqlog::LogLevel::value::INFO;
// xqlog::Formatter::ptr fmt(new xqlog::Formatter());
// xqlog::StdoutSink::ptr stds(new xqlog::StdoutSink());

// xqlog::FileSink::ptr fs(new
// xqlog::FileSink("./FileSink/logfile.log"));

// xqlog::RollFileSink::ptr rfs(
//     new xqlog::RollFileSink("./RollFileSink/logfile.log", 1024 *
//     1024));

// RollByTimeSink::ptr rts(
//     new RollByTimeSink("./roll_logfile/roll", TimeGap::GAP_SECOND));

// std::vector<xqlog::LogSink::ptr> sinks = {fs,rfs};
// std::shared_ptr<xqlog::Logger> logger(
//     new xqlog::SyncLogger(loggername, limit, fmt, sinks));

// int cur_size = 0;
// int len = sizeof("格式化功能测试...");
// while (cur_size < 1024 * 1024 * 5) {
//     logger->Error(__FILE__, __LINE__, "格式化功能测试...");
//     cur_size += len;
// }

// time_t old = time(NULL);

// while(time(nullptr) < old + 5) {
//     usleep(1000);
//     logger->Error("test", 5, "格式化功能测试...");
// }
/*Util
// std::string filename = ".././xq/xq.txt";
// cout << Util::Date::Now() << endl;

// if (Util::file::Exists(filename)) cout << "true" << endl;
// cout << 1<<endl;
// cout << Util::file::Path(filename) << endl;
// xqlog::Util::file::CreateDirectory(filename);*/
//////////////////////////////////////////////////////////////////////
// formatter
// xqlog::LogMessage msg(xqlog::LogLevel::value::INFO, "main.c", 53,
// "root",
//                       "格式化功能测试...");
// xqlog::Formatter fmt;
// // std::cout << fmt.format(msg) << std::endl;
// std::string Data = fmt.format(msg);
// xqlog::FileSink fs("./FileSink/t1.log");
// fs.Sink(Date.c_str(), Date.size());

// std::shared_ptr<xqlog::LogFactory> lf(new xqlog::LogFactory);
// std::shared_ptr<xqlog::RollFileSink> rf =
//     lf->CreateLog<xqlog::RollFileSink>("./roll_logfile",
//                                        1024 * 1024);  // 1MB
// for (int i = 0; i < 1024 * 1024 * 10; i++) {
//     rf->Sink(Date.c_str(), Date.size());
//     i += Date.size();
// }
// std::shared_ptr<xqlog::LogFactory> lf(new xqlog::LogFactory);
// std::shared_ptr<xqlog::LogSink> tf =
//     lf->CreateLog<RollByTimeSink>("./roll_logfile",
//     TimeGap::GAP_SECOND);
// time_t t = time(nullptr);
// while (time(nullptr) < t + 10) {
//     tf->Sink(Data.c_str(), Data.size());
// }