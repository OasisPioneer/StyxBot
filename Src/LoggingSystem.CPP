#include "LoggingSystem.HPP"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

// 构造函数：初始化日志系统并打开日志文件 / Constructor: Initialize the logging system and open the log file
LoggingSystem::LoggingSystem(const std::string& LogFileName) : LogFile(LogFileName, std::ios::out | std::ios::app){}

// 记录日志消息到控制台和文件 / Record log messages to console and file
// 记录日志的方法 / Method to record logs
void LoggingSystem::Log(LogLevel Level, const std::string& Message)
{
    std::lock_guard<std::mutex> Lock(LogMutex);  // 确保线程安全 / Ensure thread safety
    
    // 生成控制台日志消息（带颜色编码） / Generate console log message (with color codes)
    const std::string ConsoleLogMessage = FormatLogMessages(Level, Message, false);
    std::cout << ConsoleLogMessage << std::endl;  // 输出到控制台 / Output to console

    // 如果日志文件已打开，则写入文件（不带颜色编码） / If log file is open, write to file (without color codes)
    if (LogFile.is_open())
    {
        const std::string FileLogMessage = FormatLogMessages(Level, Message, true);
        LogFile << FileLogMessage << std::endl;  // 写入日志文件 / Write to log file
    }
}

std::string LoggingSystem::GetCurrentTime()
{
    const auto Now = std::chrono::system_clock::now();
    const auto Time = std::chrono::system_clock::to_time_t(Now);
    const std::tm TM = *std::localtime(&Time);
    std::ostringstream TimeString;
    TimeString << std::put_time(&TM, "%Y-%m-%d %H:%M:%S");
    return TimeString.str();
}

std::string LoggingSystem::GetLogLevelString(LogLevel Level)
{
    switch (Level)
    {
        case INFO:    return "[INFO]";
        case WARNING: return "[WARNING]";
        case ERROR:   return "[ERROR]";
        case DEBUG:   return "[DEBUG]";
        default:      return "[UNKNOWN]";
    }
}

std::string LoggingSystem::GetLogLevelColor(LogLevel Level)
{
    switch (Level) {
    case INFO:    return "\033[30;102m"; // 黑色字，亮绿色背景
    case WARNING: return "\033[30;103m"; // 黑色字，亮黄色背景
    case ERROR:   return "\033[37;41m"; // 白色字，红色背景
    case DEBUG:   return "\033[30;104m"; // 黑色字，亮蓝色背景
    default:      return "\033[37;40m"; // 白色字，黑色背景
    }
}

std::string LoggingSystem::FormatLogMessages(LogLevel Level, const std::string& Message, bool ForFile)
{
    std::ostringstream LogMessage;
    
    // 时间的颜色：青色字，黑色背景（科幻风格）
    const std::string TimeColorCode = "\033[96;40m";
    // 日志类型的颜色
    const std::string LogLevelColorCode = GetLogLevelColor(Level);

    const std::string MessageColorCode = GetLogLevelColor(Level);

    if (!ForFile) {
        LogMessage << TimeColorCode; // 时间颜色
    }
    LogMessage << GetCurrentTime(); // 添加时间
    
    if (!ForFile) {
        LogMessage << "\033[0m "; // 重置时间颜色
        LogMessage << LogLevelColorCode; // 日志类型颜色
    }
    LogMessage << GetLogLevelString(Level); // 添加日志类型
    
    if (!ForFile) {
        LogMessage << "\033[0m "; // 重置日志类型颜色
        LogMessage << MessageColorCode; // 消息颜色
    }

    LogMessage << Message; // 添加消息内容

    if (!ForFile) {
        LogMessage << "\033[0m"; // 重置所有颜色
    }

    return LogMessage.str();
}

LoggingSystem::~LoggingSystem()
{
    if (LogFile.is_open())
    {
        LogFile.close();
    }
}