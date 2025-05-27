#include "EventHandlerCenter.HPP"
#include "ConfigFileOperations.HPP"
#include "TelegramBotAPI.HPP"
#include "StyxSQLite.HPP"

#include <regex>
#include <thread>
#include <chrono>
#include <sstream>

#include <nlohmann/json.hpp>

EventHandlerCenter::EventHandlerCenter() : LOG("EventHandlerCenter-LOG.txt") {}

void EventHandlerCenter::Start()
{
    StyxSQLite DB("StyxSQLite.db");
    if (!DB.INIT())
    {
        LOG.Log(LoggingSystem::ERROR, "Failed to initialize StyxSQLite Database.");
        return;
    }

    int offset = 0;
    TelegramBotAPI TelegramBotAPI;
    LOG.Log(LoggingSystem::INFO, "Polling in Progress.");

    while (true)
    {
        std::string Updates = TelegramBotAPI.GetUpdates(offset);
        if (Updates.empty())
        {
            LOG.Log(LoggingSystem::INFO, "No updates received.");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }
        try
        {
            nlohmann::json Json = nlohmann::json::parse(Updates);
            if (Json.contains("result") && Json["result"].is_array())
            {
                for (const auto &UPDATE : Json["result"])
                {
                    if (UPDATE.contains("message"))
                    {
                        nlohmann::json Message = UPDATE["message"];

                        int64_t From_ID = Message["from"]["id"];    // From_ID | FromID
                        std::string FromID = std::to_string(From_ID);

                        int64_t Chat_ID = Message["chat"]["id"];    // Chat_ID | ChatID
                        std::string ChatID = std::to_string(Chat_ID);

                        std::string FromName;
                        std::string FromUserName;
                        if (Message["from"].contains("first_name") && Message["from"]["first_name"].is_string())
                        {
                            FromName = Message["from"]["first_name"].get<std::string>();
                        }
                        if (Message["from"].contains("last_name") && Message["from"]["last_name"].is_string())
                        {
                            if (!FromName.empty())
                                FromName += Message["from"]["last_name"].get<std::string>();
                        }
                        if (Message["from"].contains("username") && Message["from"]["username"].is_string())
                        {
                            FromUserName = Message["from"]["username"].get<std::string>();
                        }

                        if (!DB.AddUser(Chat_ID, FromName, FromUserName))
                        {
                            LOG.Log(LoggingSystem::ERROR, "Failed to parse JSON response.");
                        }

                        // 事件处理 - Event Handling

                        if (Message.contains("sticker") && !Message["sticker"].is_null()) // [EN]Sticker message [CN]贴纸消息
                        {
                            // std::string Sticker = Message["sticker"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("photo") && !Message["photo"].is_null()) // [EN]Photo message [CN]照片消息
                        {
                            // std::string Photo = Message["photo"][0]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("video") && !Message["video"].is_null()) // [EN]Video message [CN]视频消息
                        {
                            // std::string Video = Message["video"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("animation") && !Message["animation"].is_null()) // [EN]Animation message [CN]动画消息
                        {
                            // std::string Animation = Message["animation"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("audio") && !Message["audio"].is_null()) // [EN]Music or audio file [CN]音乐或音频文件
                        {
                            // std::string Audio = Message["audio"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("voice") && !Message["voice"].is_null()) // [EN]Voice message [CN]语音消息
                        {
                            // std::string Voice = Message["voice"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("video_note") && !Message["video_note"].is_null()) // [EN]Video note [CN]视频备注
                        {
                            // std::string VideoNote = Message["video_note"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("document") && !Message["document"].is_null()) // [EN]Document file [CN]文档文件
                        {
                            // std::string Document = Message["document"]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("location") && !Message["location"].is_null()) // [EN]Location message [CN]位置消息
                        {
                            // double Latitude = Message["location"]["latitude"].get<double>();
                            // double Longitude = Message["location"]["longitude"].get<double>();
                        }
                        else if (Message.contains("venue") && !Message["venue"].is_null()) // [EN]Venue message [CN]场地消息
                        {
                            // std::string Title = Message["venue"]["title"].get<std::string>();
                        }
                        else if (Message.contains("contact") && !Message["contact"].is_null()) // [EN]Contact message [CN]联系人消息
                        {
                            // std::string PhoneNumber = Message["contact"]["phone_number"].get<std::string>();
                        }
                        else if (Message.contains("poll") && !Message["poll"].is_null()) // [EN]Poll message [CN]投票消息
                        {
                            // std::string PollID = Message["poll"]["id"].get<std::string>();
                        }
                        else if (Message.contains("dice") && !Message["dice"].is_null()) // [EN]Dice message [CN]骰子消息
                        {
                            // std::string Emoji = Message["dice"]["emoji"].get<std::string>();
                            // int Value = Message["dice"]["value"].get<int>();
                        }
                        else if (Message.contains("new_chat_member") && !Message["new_chat_member"].is_null()) // [EN]New chat member [CN]新成员加入
                        {
                            nlohmann::json NewMember = Message["new_chat_member"];
                            if (NewMember.contains("user") && NewMember["user"].is_object())
                            {
                                NewMember = NewMember["user"];
                            }
                            std::string FirstMember = NewMember.value("first_name", "");
                        }
                        else if (Message.contains("left_chat_member") && !Message["left_chat_member"].is_null()) // [EN]Left chat member [CN]成员离开
                        {
                            nlohmann::json LeftMember = Message["left_chat_member"];
                            if (LeftMember.contains("user") && LeftMember["user"].is_object())
                            {
                                LeftMember = LeftMember["user"];
                            }
                            std::string FirstMember = LeftMember.value("first_name", "");
                        }
                        else if (Message.contains("new_chat_title") && !Message["new_chat_title"].is_null()) // [EN]New chat title [CN]新群聊名称
                        {
                            std::string NewChatTitle = Message["new_chat_title"].get<std::string>();
                        }
                        else if (Message.contains("new_chat_photo") && !Message["new_chat_photo"].is_null()) // [EN]New chat photo [CN]新群聊头像
                        {
                            // std::string NewChatPhoto = Message["new_chat_photo"][0]["file_id"].get<std::string>();
                        }
                        else if (Message.contains("delete_chat_photo") && !Message["delete_chat_photo"].is_null()) // [EN]Delete chat photo [CN]删除群聊头像
                        {

                        }
                        else if (Message.contains("group_chat_created") && !Message["group_chat_created"].is_null()) // [EN]Group chat created [CN]群聊创建成功
                        {

                        }
                        else if (Message.contains("supergroup_chat_created") && !Message["supergroup_chat_created"].is_null()) // [EN]Supergroup chat created [CN]超级群创建成功
                        {

                        }
                        else if (Message.contains("channel_chat_created") && !Message["channel_chat_created"].is_null()) // [EN]Channel chat created [CN]频道创建成功
                        {

                        }
                        else if (Message.contains("migrate_to_chat_id") && !Message["migrate_to_chat_id"].is_null()) // [EN]Migrated to supergroup [CN]群迁移到超级群
                        {
                            // long MigrateToChatID = Message["migrate_to_chat_id"].get<long>();
                        }
                        else if (Message.contains("migrate_from_chat_id") && !Message["migrate_from_chat_id"].is_null()) // [EN]Migrated from supergroup [CN]超级群迁移回来
                        {
                            // long MigrateFromChatID = Message["migrate_from_chat_id"].get<long>();
                        }
                        else if (Message.contains("pinned_message") && !Message["pinned_message"].is_null())
                        {
                            // std::string PinnedMessage = Message["pinned_message"]["text"].get<std::string>();
                        }
                        else if (Message.contains("text") && !Message["text"].is_null())
                        {
                            // std::string Text = Message["text"].get<std::string>();
                            std::string Text = Message.value("text", "");

                            std::ostringstream LogStream;
                            LogStream << "UserName: " << FromName << " UserAccount: " << FromUserName
                                      << " UserID: " << FromID << " Text: " << Text;
                            LOG.Log(LoggingSystem::INFO, LogStream.str());

                            /*
                             * 通过正则判断用户输入的指令是否带有参数 如果带有参数则进入有参处理
                             */
                            std::regex Command_Regex(R"^(\\/[A-Za-z]+)(?=[^A-Za-z]|$)(?:@\\w+)?\\s*(.*)$");
                            std::smatch Match;
                            if (!std::regex_match(Text, Match, Command_Regex))
                                continue;
                            std::string Command = Match[1];
                            std::string Args = Match[2];

                            LOG.Log(LoggingSystem::DEBUG, "Command= "+ Command + " Args= " + Args);





















                        } else
                        {
                            LOG.Log(LoggingSystem::WARNING, "Unknown Type");
                            continue;
                        }
                    }
                }
                if (!Json["result"].empty()) {
                    offset = static_cast<int>(Json["result"].back()["update_id"]) + 1;
                }
            } else {
                LOG.Log(LoggingSystem::ERROR, "Invalid or missing 'result' key in JSON.");
            }
        } catch (const nlohmann::json::parse_error &E) {
            LOG.Log(LoggingSystem::ERROR, std::string("JSON parse error: ") + E.what());
            continue;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
