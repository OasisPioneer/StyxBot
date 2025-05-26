#ifndef LOGGING_SYSTEM_HPP
#define LOGGING_SYSTEM_HPP

#include <string>
#include <fstream>
#include <mutex>

// 日志系统类，用于处理日志记录 / Logging system class for handling log recording
class LoggingSystem
{
public:
    // 日志级别枚举类型，包括INFO、WARNING、ERROR和DEBUG / Log level enumeration including INFO, WARNING, ERROR, and DEBUG
    enum LogLevel{INFO, WARNING, ERROR, DEBUG};

    // 构造函数，初始化日志文件 / Constructor to initialize the log file
    explicit LoggingSystem(const std::string& LogFileName);

    // 记录日志的方法 / Method to record logs
    void Log(LogLevel Level, const std::string& Message);

    // 析构函数，关闭日志文件 / Destructor to close the log file
    ~LoggingSystem();

private:
    // 获取当前时间 / Get current time
    static std::string GetCurrentTime();

    // 根据日志级别获取日志字符串 / Get log string based on log level
    static std::string GetLogLevelString(LogLevel Level);

    // 根据日志级别获取颜色代码 / Get color code based on log level
    static std::string GetLogLevelColor(LogLevel Level);

    // 格式化日志消息 / Format log messages
    static std::string FormatLogMessages(LogLevel Level, const std::string& Message, bool ForFile = false);

    std::ofstream LogFile; // 日志文件流对象 / Log file stream object
    std::mutex LogMutex;   // 互斥锁，用于线程安全 / Mutex for thread safety
};

#endif // LOGGING_SYSTEM_HPP