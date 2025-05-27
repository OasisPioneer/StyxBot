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

    bool AddButton(const Button& Button);   // 添加按钮
    bool UpdateButton(const Button& Button);// 更新按钮
    bool RemoveButton(int ButtonDataID);        // 删除按钮
    std::vector<Button> ListButton();       // 列出按钮

    ~StyxSQLite();
private:
    sqlite3* SQLiteDB = nullptr;
    std::string SQLiteFilePath;
    LoggingSystem LOG;

    bool ExecuteCommand(const std::string& SQL);
};

#endif // STYX_SQLITE_HPP