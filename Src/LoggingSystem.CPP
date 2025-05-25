#include "LoggingSystem.HPP"

#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

LoggingSystem::LoggingSystem(const std::string& LogFileName) : LogFile(LogFileName, std::ios::out | std::ios::app){}

void LoggingSystem::Log(LogLevel Level, const std::string& Message)
{
    std::lock_guard<std::mutex> Lock(LogMutex);
    const std::string LogMessage = FormatLogMessages(Level, Message);
    std::cout << LogMessage << std::endl;
    if (LogFile.is_open())
    {
        LogFile << LogMessage << std::endl;
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
    case INFO:    return "\033[32m";
    case WARNING: return "\033[33m";
    case ERROR:   return "\033[31m";
    case DEBUG:   return "\033[34m";
    default:      return "\033[37m";
    }
}

std::string LoggingSystem::FormatLogMessages(LogLevel Level, const std::string& Message)
{
    std::ostringstream LogMessage;
    LogMessage << GetCurrentTime() << " " << GetLogLevelString(Level) << GetLogLevelColor(Level) << " " << Message << "\033[0m";
    return LogMessage.str();
}

LoggingSystem::~LoggingSystem()
{
    if (LogFile.is_open())
    {
        LogFile.close();
    }
}