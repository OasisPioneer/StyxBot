#ifndef LOGGING_SYSTEM_HPP
#define LOGGING_SYSTEM_HPP

#include <string>
#include <fstream>
#include <mutex>

class LoggingSystem
{
public:
    enum LogLevel{INFO, WARNING, ERROR, DEBUG};
    explicit LoggingSystem(const std::string& LogFileName);

    void Log(LogLevel Level, const std::string& Message);

    ~LoggingSystem();
private:
    static std::string GetCurrentTime();
    static std::string GetLogLevelString(LogLevel Level);
    static std::string GetLogLevelColor(LogLevel Level);
    static std::string FormatLogMessages(LogLevel Level, const std::string& Message);

    std::ofstream LogFile;
    std::mutex LogMutex;
};

#endif // LOGGING_SYSTEM_HPP