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

    ~StyxSQLite();
private:
    sqlite3* SQLiteDB = nullptr;
    std::string SQLiteFilePath;
    LoggingSystem LOG;

    bool ExecuteCommand(const std::string& SQL);
};

#endif // STYX_SQLITE_HPP