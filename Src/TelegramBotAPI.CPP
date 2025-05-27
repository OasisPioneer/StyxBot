#include "TelegramBotAPI.HPP"
#include "ConfigFileOperations.HPP"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

TelegramBotAPI::TelegramBotAPI() : LOG("TelegramBotAPI-LOG.txt"), Net()
{
    auto Token = ReadConfigFile<std::string>("ConfigFile.Json", "TelegramBotToken");
    if (Token.has_value())
    {
        TelegramBotToken = Token.value();
        TelegramBotURL = "https://api.telegram.org/bot" + TelegramBotToken + "/";
    } else
    {
        TelegramBotToken = "";
        TelegramBotURL = "";
        LOG.Log(LoggingSystem::WARNING, "Failed to load TelegramBotToken from configuration file. Using empty token.");
        return;
    }
}

std::string TelegramBotAPI::GetBotName()
{
    try
    {
        const std::string URL = TelegramBotURL + "getMe";
        std::string Res = Net.HTTP_GET(URL);
        nlohmann::json Json = nlohmann::json::parse(Res);
        // if (!Json.value("ok", true))
        if (!Json.contains("ok") || !Json["ok"].get<bool>())
        {
            LOG.Log(LoggingSystem::ERROR, "getMe API returned 'ok' as false.");
            throw std::runtime_error("getMe Return Not OK");
        }
        return Json["result"].value("username", "");
    }
    catch (const nlohmann::json::parse_error& E)
    {
        LOG.Log(LoggingSystem::ERROR, "JSON Parse Error: " + std::string(E.what()));
        throw std::runtime_error("Failed to parse JSON response.");
    }
    catch (const std::exception& E)
    {
        LOG.Log(LoggingSystem::ERROR, "Network Request Error: " + std::string(E.what()));
        throw std::runtime_error("Network request failed.");
    }

}

std::string TelegramBotAPI::GetUpdates(int offset)
{
    try
    {
        const std::string URL = TelegramBotURL + "getUpdates?timeout=5&offset=" + std::to_string(offset);
        std::string Response = Net.HTTP_GET(URL);
        if (Response.empty()) {
            LOG.Log(LoggingSystem::ERROR, "Empty response from Telegram API.");
        }
        return Response;
    }
    catch (const std::exception &E)
    {
        LOG.Log(LoggingSystem::ERROR, "Network request failed: " + std::string(E.what()));
        return "";
    }
}

std::string TelegramBotAPI::SendMessage(const std::string& UserID, const std::string& Message)
{
    std::string URL = TelegramBotURL + "sendMessage";
    char* EscapedText = curl_easy_escape(nullptr, Message.c_str(), Message.length());
    std::string POSTField = "chat_id=" + UserID + "&text=" + std::string(EscapedText);
    curl_free(EscapedText);
    return Net.HTTP_POST(URL, POSTField);
}


TelegramBotAPI::~TelegramBotAPI()
{
    LOG.Log(LoggingSystem::INFO, "Exiting...");
}