#ifndef STYX_SQLITE_HPP
#define STYX_SQLITE_HPP

#include <string>
#include <vector>

#include "LoggingSystem.HPP"

#include <sqlite3.h>

struct Button
{
    int ID;                 // 按钮ID
    std::string Type;       // 按钮类型
    std::string Title;      // 按钮文本
    std::string Data;       // 按钮数据: 文字或链接
};

class StyxSQLite
{
public:
    explicit StyxSQLite(std::string  SQLitePath);

    bool INIT();

    ~StyxSQLite();
private:
    sqlite3* SQLiteDB = nullptr;
    std::string SQLiteFilePath;
    LoggingSystem LOG;

    bool ExecuteCommand(const std::string& SQL);
};

#endif // STYX_SQLITE_HPP