#ifndef TELEGRAM_BOT_API_HPP
#define TELEGRAM_BOT_API_HPP

#include <string>

#include "LoggingSystem.HPP"
#include "NetworkRequest.HPP"

class TelegramBotAPI
{
public:
    TelegramBotAPI();

    std::string GetBotName();

    std::string GetUpdates(int offset);
    std::string SendMessage(const std::string& UserID, const std::string& Message);

    ~TelegramBotAPI();
private:
    std::string TelegramBotToken;
    std::string TelegramBotURL;
    LoggingSystem LOG;
    NetworkRequest Net;
};

#endif // TELEGRAM_BOT_API_HPP
