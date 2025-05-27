#ifndef STYX_SQLITE_HPP
#define STYX_SQLITE_HPP

#include <string>
#include <vector>

#include "LoggingSystem.HPP"

#include <sqlite3.h>

struct Button
{
    int ID;
    std::string Type;
    std::string Title;
    std::string Data;
    std::string CommandType;
};

struct ADS
{
    int ID;
    std::string Title;
    std::string URL;
};

class StyxSQLite
{
public:
    explicit StyxSQLite(std::string  SQLitePath);

    bool INIT();

    // 按钮接口
    bool AddButton(const Button& Button);
    bool UpdateButton(const Button& Button);
    bool RemoveButton(int ButtonDataID);
    std::vector<Button> ListButton() const;

    // 广告接口
    bool AddAd(const ADS& AD);
    bool RemoveAd(int AdDataID);
    std::vector<ADS> ListAD() const;

    // 管理员接口
    bool AddAdmin(long long UserID);
    bool RemoveAdmin(long long UserID);
    bool IsAdmin(long long UserID);
    std::vector<long long> ListAdmin() const;

    // 用户接口
    bool AddUser(long long UserID, const std::string& FromName, const std::string& FromUserName);
    bool AddBalance(long long UserID, int Balance);
    bool DeductBalance(long long UserID, int Balance);
    bool Signin(long long UserID);
    int CheckBalance(long long UserID);
    bool SetInvite(long long UserID, long long InviteID);
    long long GetInviteID(long long UserID) const;
    int GetInviteNumberUsers(long long UserID) const;

    // 群组/频道接口
    bool AddGroup(long long ChatID);
    bool RemoveGroup(long long ChatID);
    bool IsGroup(long long ChatID) const;
    std::vector<long long> ListGroup() const;

    // 系统处理
    bool GetUserIDFromUserName(const std::string& UserName, long long& OutUserID) const;

    ~StyxSQLite();
private:
    sqlite3*        SQLiteDB        =   nullptr;
    std::string     SQLiteFilePath;
    LoggingSystem   LOG;

    bool ExecuteCommand(const std::string& SQL);
    bool PrepareAndExecute(const std::string& SQL, const std::vector<std::string>& params = {});
};

#endif // STYX_SQLITE_HPP

