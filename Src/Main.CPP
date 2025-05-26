#include <iostream>
#include "LoggingSystem.HPP"
#include "ConfigFileOperations.HPP"
#include "EventHandlerCenter.HPP"

const std::string ADMIN_ID_KEY = "AdministratorIDCard";
const std::string TELEGRAM_TOKEN_KEY = "TelegramBotToken";

// 主函数入口 / Main function entry point
int main(int argc, char* argv[])
{
    // 配置文件路径 / Configuration file path
    const std::string ConfigFilePath = "ConfigFile.Json";
    // 初始化日志系统 / Initialize logging system
    LoggingSystem LOG("System-LOG.txt");
    
    // 检查配置文件路径是否正确，如果不存在则创建默认配置文件
    if (!std::filesystem::exists(ConfigFilePath) || std::filesystem::is_empty(ConfigFilePath)) {
        LOG.Log(LoggingSystem::WARNING, "Configuration file does not exist or is empty at the expected location: " + ConfigFilePath);
        LOG.Log(LoggingSystem::INFO, "Creating a default configuration file.");

        // 创建默认配置内容
        const std::string DefaultConfigContent = R"({
            "AdministratorIDCard": "",
            "TelegramBotToken": ""
        })";

        // 写入默认配置到文件
        std::ofstream DefaultConfigFile(ConfigFilePath);
        if (DefaultConfigFile.is_open()) {
            DefaultConfigFile << DefaultConfigContent;
            DefaultConfigFile.close();
            LOG.Log(LoggingSystem::INFO, "Default configuration file created successfully.");
        } else {
            LOG.Log(LoggingSystem::ERROR, "Failed to create default configuration file.");
        }
    }

    // 如果没有提供命令行参数，则提示用户使用帮助命令 / If no command line arguments are provided, prompt the user to use the help command
    if (argc < 2)
    {
        LOG.Log(LoggingSystem::ERROR, "No Command Provided!"); // 提示未提供命令 / Prompt that no command was provided
        LOG.Log(LoggingSystem::INFO, "Please Add Command --H to Get Help!");
        // 提示用户输入帮助命令 / Prompt the user to input the help command
    }

    // 遍历命令行参数 / Iterate through command line arguments
    for (int i = 1; i < argc; i++)
    {
        std::string Command = argv[i]; // 当前命令 / Current command

        // 处理管理员ID设置命令 / Handle admin ID setting command
        if (Command == "--A" || Command == "--ADMIN")
        {
            if (i + 1 < argc) // 检查是否有后续参数 / Check if there is a subsequent parameter
            {
                try
                {
                    long long AdminID = std::stoll(argv[i + 1]); // 将字符串转换为整数 / Convert string to integer
                    LOG.Log(LoggingSystem::DEBUG, std::to_string(AdminID)); // 记录调试信息 / Record debug information
                    if (WriteConfigFile(ConfigFilePath, ADMIN_ID_KEY, AdminID))
                    // 写入配置文件 / Write to configuration file
                    {
                        LOG.Log(LoggingSystem::INFO, "Set Administrator ID Card Success as Integer");
                        // 设置成功 / Set successfully
                    }
                }
                catch (const std::exception& E)
                {
                    LOG.Log(LoggingSystem::ERROR, "Invalid Administrator ID: " + std::string(E.what()));
                    // 无效管理员ID / Invalid admin ID
                    LOG.Log(LoggingSystem::ERROR,
                            "Set Administrator ID Card Failed Failed to write to file: " + ConfigFilePath
                    ); // 写入失败 / Write failure
                }
                i++; // 跳过值参数 / Skip value parameter
            }
            else
            {
                LOG.Log(LoggingSystem::ERROR, "Missing argument for command"); // 缺少参数 / Missing argument
            }
        }
        // 处理Telegram机器人令牌设置命令 / Handle Telegram bot token setting command
        else if (Command == "--T" || Command == "--TOKEN")
        {
            if (i + 1 < argc) // 检查是否有后续参数 / Check if there is a subsequent parameter
            {
                LOG.Log(LoggingSystem::DEBUG, argv[i + 1]); // 记录调试信息 / Record debug information
                if (WriteConfigFile(ConfigFilePath, TELEGRAM_TOKEN_KEY, argv[i + 1]))
                // 写入配置文件 / Write to configuration file
                {
                    LOG.Log(LoggingSystem::INFO,
                            "Set Telegram Bot Token Success"
                    ); // 设置成功 / Set successfully
                }
                else
                {
                    LOG.Log(LoggingSystem::ERROR, "Set Telegram Bot Token Failed"); // 设置失败 / Set failed
                }
                i++; // 跳过值参数 / Skip value parameter
            }
            else
            {
                LOG.Log(LoggingSystem::ERROR, "Missing argument for command"); // 缺少参数 / Missing argument
            }
        }
        // 显示帮助信息 / Display help information
        else if (Command == "--H" || Command == "--HELP")
        {
            std::string HELPINFO;
            HELPINFO = "⇐========================================⇒\n";
            HELPINFO += "‖ [How to use] " + std::string(argv[0]) + " --Command\n"; // 如何使用 / How to use
            HELPINFO += "‖ --H Or --HELP [Get Help]\n"; // 获取帮助 / Get help
            HELPINFO += "‖ --A Or --ADMIN [Set Telegram Bot Admin]\n"; // 设置管理员 / Set admin
            HELPINFO += "‖ --T Or --TOKEN [Set Telegram Bot Token]\n"; // 设置令牌 / Set token
            HELPINFO += "⇐========================================⇒";
            std::cout << HELPINFO << std::endl; // 输出帮助信息 / Output help information
        } else if (Command == "--S" || Command == "--START")
        {
            LOG.Log(LoggingSystem::INFO, "System Self-Checking...");

            try {
                auto AdminID = ReadConfigFile<long long>(ConfigFilePath, ADMIN_ID_KEY);
                auto TelegramBotToken = ReadConfigFile<std::string>(ConfigFilePath, TELEGRAM_TOKEN_KEY);
            
                if (!AdminID.has_value() || AdminID.value() == 0 || 
                    !TelegramBotToken.has_value() || TelegramBotToken.value().empty())
                {
                    if (!AdminID.has_value() || AdminID.value() == 0) {
                        LOG.Log(LoggingSystem::WARNING, "Administrator ID is not set or is empty in the configuration file!");
                    }
                    if (!TelegramBotToken.has_value() || TelegramBotToken.value().empty()) {
                        LOG.Log(LoggingSystem::WARNING, "Telegram Bot Token is not set or is empty in the configuration file!");
                    }
                    LOG.Log(LoggingSystem::WARNING, "Configuration file path: " + ConfigFilePath);
                    LOG.Log(LoggingSystem::WARNING, "Please ensure the configuration file exists and contains valid JSON data.");
                    LOG.Log(LoggingSystem::WARNING, "Use --A/--ADMIN and --T/--TOKEN to manually set missing values.");
                    return 1; // 结束程序
                }
            } catch (const std::exception& e) {
                LOG.Log(LoggingSystem::ERROR, "Configuration File Loading Failed: " + std::string(e.what()));
                LOG.Log(LoggingSystem::WARNING, "Configuration file path: " + ConfigFilePath);
                LOG.Log(LoggingSystem::WARNING, "Please ensure the configuration file exists and contains valid JSON data.");
                return 1; // 结束程序
            }

            EventHandlerCenter EventHandlerCenter;
            EventHandlerCenter.Start();
        }
        else
        {
            LOG.Log(LoggingSystem::ERROR, "Error: Unknown command '" + Command + "'"); // 未知命令 / Unknown command
            LOG.Log(LoggingSystem::INFO, "Use --H or --HELP to get help."); // 提示使用帮助命令 / Prompt to use the help command
        }
    }

    return 0; // 返回成功状态码 / Return success status code
}