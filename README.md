# Styx Bot

> A lightweight, high-performance Telegram Bot framework written in modern C++ using LibCURL and SQLite.

## Configuration

To properly set up the bot, please ensure you have a valid configuration file. The configuration file should be in JSON format and placed in the working directory of the bot. If the configuration file is missing or empty, the bot will automatically create a default configuration file with empty values for `AdministratorIDCard` and `TelegramBotToken`. You need to manually edit this file to fill in the required information.

Example configuration file content:
```json
{
    "AdministratorIDCard": "7125495680",
    "TelegramBotToken": "7491395783:AAFRMKiA-ZzBXID-CYuLRuw15cWsTAP4pcc"
}