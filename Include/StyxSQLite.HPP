#ifndef STYX_SQLITE_HPP
#define STYX_SQLITE_HPP

#include <string>
#include <vector>

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
    StyxSQLite();
    ~StyxSQLite();

    // 创建数据库 / Create database
    bool CreateDatabase(const std::string& DatabaseName);

    // 创建表 / Create table
    bool CreateTable(const std::string& TableName, const std::string& TableSchema);

    // 插入数据 / Insert data
    bool InsertData(const std::string& TableName, const std::string& Data);
private:
    sqlite3* SQLiteDB = nullptr;
    std::string SQLiteFilePath;
};

#endif // STYX_SQLITE_HPP