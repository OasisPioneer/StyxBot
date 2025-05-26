#ifndef CONFIG_FILE_OPERATIONS_HPP
#define CONFIG_FILE_OPERATIONS_HPP

#include <string>
#include <optional>
#include <fstream>
#include <mutex>

#include "LoggingSystem.HPP"
#include <nlohmann/json.hpp>

static LoggingSystem LOG("ConfigFile Log.txt");
static std::mutex LogMutex;

// 将配置项写入指定路径的 JSON 文件 / Write configuration item to a JSON file at the specified path
template <typename T>
bool WriteConfigFile(const std::string& Path, const std::string& Key, const T& Value)
{
    std::lock_guard<std::mutex> Lock(LogMutex);  // 确保线程安全

    nlohmann::json Config;  // JSON 对象，用于存储配置
    std::ifstream InputFile(Path);  // 打开文件以读取现有配置
    if (InputFile.is_open())
    {
        try
        {
            std::string Content((std::istreambuf_iterator<char>(InputFile)), std::istreambuf_iterator<char>());
            if (Content.empty() || !nlohmann::json::accept(Content))
            {
                LOG.Log(LoggingSystem::INFO, "Configuration File is Empty or Invalid JSON Format, Initializing...");
                Config = nlohmann::json::object();
            }
            else
            {
                Config = nlohmann::json::parse(Content);
                LOG.Log(LoggingSystem::INFO, "Configuration File Loaded Successfully");
            }
        }
        catch (const std::exception& E)
        {
            LOG.Log(LoggingSystem::ERROR, "Configuration File Loading Failed: " + std::string(E.what()));
            InputFile.close();
            return false;
        }
        InputFile.close();
    }
    else
    {
        LOG.Log(LoggingSystem::INFO, "The Configuration File Does Not Exist, Creating a New One");
        Config = nlohmann::json::object();
    }

    nlohmann::json* Current = &Config;
    size_t Pos = 0, Next;
    while ((Next = Key.find(".", Pos)) != std::string::npos)
    {
        std::string SubKey = Key.substr(Pos, Next - Pos);
        if ((*Current)[SubKey].is_null())
        {
            (*Current)[SubKey] = nlohmann::json::object();
        }
        Current = &(*Current)[SubKey];
        Pos = Next + 1;
    }

    std::string FinalKey = Key.substr(Pos);
    (*Current)[FinalKey] = Value;

    std::ofstream OutputFile(Path); // 使用默认覆盖模式
    if (!OutputFile)
    {
        LOG.Log(LoggingSystem::ERROR, "Configuration File Saving Failed");
        return false;
    }

    OutputFile << Config.dump(4) << std::endl; // 写入格式化后的 JSON 数据
    if (!OutputFile.good())
    {
        LOG.Log(LoggingSystem::ERROR, "Failed to Write to Configuration File");
        OutputFile.close();
        return false;
    }

    OutputFile.close();
    LOG.Log(LoggingSystem::INFO, "Configuration File Updated Successfully");
    return true;
}

        template <typename T>
        std::optional<T> ReadConfigFile(const std::string& Path, const std::string& Key)
        {
            std::lock_guard<std::mutex> Lock(LogMutex);

            nlohmann::json Config;
            std::ifstream InputFile(Path);
            if (InputFile.is_open())
            {
                try
                {
        std::string Content((std::istreambuf_iterator<char>(InputFile)), std::istreambuf_iterator<char>());
        if (Content.empty() || !nlohmann::json::accept(Content))
        {
            LOG.Log(LoggingSystem::ERROR, "Invalid or Empty JSON Format in Configuration File");
            return std::nullopt;
        }
        try {
            Config = nlohmann::json::parse(Content);
            LOG.Log(LoggingSystem::INFO, "Configuration File Loaded Successfully");
        } catch (const std::exception& E) {
            LOG.Log(LoggingSystem::ERROR, "Configuration File Parsing Failed: " + std::string(E.what()));
            return std::nullopt;
        }
                } catch (const std::exception& E)
                {
        LOG.Log(LoggingSystem::ERROR, "Configuration File Loading Failed" + std::string(E.what()));
        return std::nullopt;
                }
                InputFile.close();
            } else
            {
                LOG.Log(LoggingSystem::INFO, "The Configuration File Does Not Exist, Creating a New One");
                std::ofstream OutputFile(Path);
                if (!OutputFile)
                {
        LOG.Log(LoggingSystem::ERROR, "Configuration File Creation Failed");
        return std::nullopt;
        }
        OutputFile << Config.dump(4) << std::endl;
        OutputFile.close();
        LOG.Log(LoggingSystem::INFO, "Configuration File Created Successfully");
        return std::nullopt;
    }

    nlohmann::json* Current = &Config;
    size_t Pos = 0, Next;

    while ((Next = Key.find('.', Pos)) != std::string::npos)
    {
        std::string SubKey = Key.substr(Pos, Next - Pos);
        if ((*Current)[SubKey].is_null())
        {
            LOG.Log(LoggingSystem::WARNING, "Configuration Key Does Not Exist: " + Key);
            return std::nullopt;
        }
        Current = &((*Current)[SubKey]);
        Pos = Next + 1;
    }

    std::string FinalKey = Key.substr(Pos);
    if (Current->contains(FinalKey))
    {
        try {
            if constexpr (std::is_same_v<T, std::string>) {
                return (*Current)[FinalKey].get<std::string>();
            } else {
                return (*Current)[FinalKey].get<T>();
            }
        } catch (const nlohmann::json::type_error& E) {
            LOG.Log(LoggingSystem::ERROR, "Type mismatch in configuration file for key: " + Key + ", expected type: " + typeid(T).name());
            return std::nullopt;
        }
    }

    LOG.Log(LoggingSystem::WARNING, "Configuration Key Does Not Exist: " + Key);
    return std::nullopt;
}

#endif // CONFIG_FILE_OPERATIONS_HPP