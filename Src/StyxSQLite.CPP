#include "StyxSQLite.HPP"
#include <thread>
#include <ctime>
#include <iomanip>

StyxSQLite::StyxSQLite(std::string  SQLitePath)
            : SQLiteDB(nullptr)
            , SQLiteFilePath(std::move(SQLitePath))
            , LOG("SQLite-LOG.txt")
{}

bool StyxSQLite::INIT() {
    if (sqlite3_open(SQLiteFilePath.c_str(), &SQLiteDB) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "Unable to open database: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    sqlite3_busy_timeout(SQLiteDB, 5000);
    if (!ExecuteCommand("BEGIN TRANSACTION;")) return false;

    const std::vector<std::string> TableStatements = {
        "CREATE TABLE IF NOT EXISTS ADMIN (UserID INTEGER PRIMARY KEY);",
        "CREATE TABLE IF NOT EXISTS USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT, UserID INTEGER UNIQUE, FromName TEXT, FromUserName TEXT UNIQUE, Balance INTEGER DEFAULT 0, InviteID INTEGER NULL, Violations INTEGER DEFAULT 0);",
        "CREATE TABLE IF NOT EXISTS SignIn (UserID INTEGER NOT NULL, SignDate TEXT NOT NULL, Timestamp INTEGER NOT NULL, PRIMARY KEY (UserID, SignDate));",
        "CREATE TABLE IF NOT EXISTS GROUPS (ChatID INTEGER PRIMARY KEY);",
        "CREATE TABLE IF NOT EXISTS USER_GROUP (UserID INTEGER NOT NULL, ChatID INTEGER NOT NULL, PRIMARY KEY(UserID, ChatID));",
        "CREATE TABLE IF NOT EXISTS BUTTON (ID INTEGER PRIMARY KEY AUTOINCREMENT, Type TEXT NOT NULL, Title TEXT NOT NULL, Data TEXT NOT NULL, CommandType TEXT NOT NULL);",
        "CREATE TABLE IF NOT EXISTS ADS (ID INTEGER PRIMARY KEY AUTOINCREMENT, Title TEXT NOT NULL, URL TEXT NOT NULL);"
    };

    for (const auto& SQL : TableStatements) {
        if (!ExecuteCommand(SQL)) {
            ExecuteCommand("ROLLBACK;");
            LOG.Log(LoggingSystem::ERROR, "[INIT] Failed to execute SQL: " + SQL);
            return false;
        }
    }

    if (!ExecuteCommand("COMMIT;")) {
        LOG.Log(LoggingSystem::ERROR, "[INIT] COMMIT failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    return true;
}

bool StyxSQLite::ExecuteCommand(const std::string& SQL) {
    char* ErrMSG = nullptr;
    int Result = sqlite3_exec(SQLiteDB, SQL.c_str(), nullptr, nullptr, &ErrMSG);
    if (Result != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "SQL Exec Error: " + std::string(ErrMSG));
        sqlite3_free(ErrMSG);
        return false;
    }
    return true;
}

bool StyxSQLite::PrepareAndExecute(const std::string& SQL, const std::vector<std::string>& params) {
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "Prepare Error: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    for (size_t i = 0; i < params.size(); ++i) {
        sqlite3_bind_text(STMT, static_cast<int>(i + 1), params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    bool OK = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!OK) {
        LOG.Log(LoggingSystem::ERROR, "Execute Step Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
    }

    sqlite3_finalize(STMT);
    return OK;
}

bool StyxSQLite::AddButton(const Button& Button) {
    const std::string SQL =
        "INSERT INTO BUTTON(Type, Title, Data, CommandType) VALUES(?, ?, ?, ?);";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "AddButton Prepare Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_text(STMT, 1, Button.Type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 2, Button.Title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 3, Button.Data.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 4, Button.CommandType.c_str(), -1, SQLITE_TRANSIENT);
    bool ok = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!ok) LOG.Log(LoggingSystem::ERROR, "AddButton Exec Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
    sqlite3_finalize(STMT);
    return ok;
}

bool StyxSQLite::UpdateButton(const Button& Button) {
    const std::string SQL =
        "UPDATE BUTTON SET Type = ?, Title = ?, Data = ?, CommandType = ? WHERE ID = ?;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "UpdateButton Prepare Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_text(STMT, 1, Button.Type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 2, Button.Title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 3, Button.Data.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 4, Button.CommandType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(STMT, 5, Button.ID);
    bool ok = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!ok) LOG.Log(LoggingSystem::ERROR, "UpdateButton Exec Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
    sqlite3_finalize(STMT);
    return ok;
}

bool StyxSQLite::RemoveButton(int ButtonDataID) {
    return PrepareAndExecute("DELETE FROM BUTTON WHERE ID = "+std::to_string(ButtonDataID)+";");
}

std::vector<Button> StyxSQLite::ListButton() const
{
    const char* SQL = "SELECT ID, Type, Title, Data, CommandType FROM BUTTON;";
    sqlite3_stmt* STMT = nullptr;
    std::vector<Button> vec;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) == SQLITE_OK) {
        while (sqlite3_step(STMT) == SQLITE_ROW) {
            Button BTN;
            BTN.ID          = sqlite3_column_int(STMT, 0);
            BTN.Type        = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 1));
            BTN.Title       = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 2));
            BTN.Data        = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 3));
            BTN.CommandType = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 4));
            vec.push_back(std::move(BTN));
        }
    }
    sqlite3_finalize(STMT);
    return vec;
}

bool StyxSQLite::AddAd(const ADS& AD) {
    return PrepareAndExecute(
        "INSERT INTO ADS(Title, URL) VALUES(?, ?);",
        { AD.Title, AD.URL }
    );
}

bool StyxSQLite::RemoveAd(int AdDataID) {
    return PrepareAndExecute("DELETE FROM ADS WHERE ID = "+std::to_string(AdDataID)+";");
}

std::vector<ADS> StyxSQLite::ListAD() const
{
    const char* SQL = "SELECT ID, Title, URL FROM ADS;";
    sqlite3_stmt* STMT = nullptr;
    std::vector<ADS> vec;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) == SQLITE_OK) {
        while (sqlite3_step(STMT) == SQLITE_ROW) {
            ADS a;
            a.ID    = sqlite3_column_int(STMT, 0);
            a.Title = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 1));
            a.URL   = reinterpret_cast<const char*>(sqlite3_column_text(STMT, 2));
            vec.push_back(std::move(a));
        }
    }
    sqlite3_finalize(STMT);
    return vec;
}

bool StyxSQLite::AddAdmin(long long UserID) {
    return PrepareAndExecute("INSERT OR IGNORE INTO ADMIN(UserID) VALUES(" + std::to_string(UserID) + ");");
}

bool StyxSQLite::RemoveAdmin(long long UserID) {
    return PrepareAndExecute("DELETE FROM ADMIN WHERE UserID = "+std::to_string(UserID)+";");
}

bool StyxSQLite::IsAdmin(long long UserID) {
    sqlite3_stmt* STMT = nullptr;
    const char* SQL = "SELECT 1 FROM ADMIN WHERE UserID = ? LIMIT 1;";
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "IsAdmin Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_int64(STMT, 1, UserID);
    bool Found = (sqlite3_step(STMT) == SQLITE_ROW);
    sqlite3_finalize(STMT);
    return Found;
}

std::vector<long long> StyxSQLite::ListAdmin() const
{
    sqlite3_stmt* STMT = nullptr;
    std::vector<long long> IDS;
    const char* SQL = "SELECT UserID FROM ADMIN;";
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) == SQLITE_OK) {
        while (sqlite3_step(STMT) == SQLITE_ROW) {
            IDS.push_back(sqlite3_column_int64(STMT, 0));
        }
    }
    sqlite3_finalize(STMT);
    return IDS;
}

bool StyxSQLite::AddUser(long long UserID, const std::string& FromName, const std::string& FromUserName) {
    const char* SQL = "INSERT OR IGNORE INTO USERS(UserID, FromName, FromUserName) VALUES(?, ?, ?);";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "AddUser Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_int64(STMT, 1, UserID);
    sqlite3_bind_text(STMT, 2, FromName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 3, FromUserName.c_str(), -1, SQLITE_TRANSIENT);
    bool Result = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!Result) LOG.Log(LoggingSystem::ERROR, "AddUser Execution Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
    sqlite3_finalize(STMT);
    return Result;
}

bool StyxSQLite::AddBalance(long long UserID, int Balance) {
    const char* SQL =
        "INSERT INTO USERS(UserID, Balance) VALUES(?, ?)\n"
        "  ON CONFLICT(UserID) DO UPDATE SET Balance = Balance + excluded.Balance;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "AddBalance Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    if (sqlite3_bind_int64(STMT, 1, UserID) != SQLITE_OK || sqlite3_bind_int(STMT, 2, Balance) != SQLITE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "AddBalance Error= " + std::string(sqlite3_errmsg(SQLiteDB)));
        sqlite3_finalize(STMT);
        return false;
    }
    bool Result = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!Result) LOG.Log(LoggingSystem::ERROR, "AddBalance Execution Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
    sqlite3_finalize(STMT);
    return Result;
}

bool StyxSQLite::DeductBalance(long long UserID, int Balance) {
    const std::string SQL = R"(
        UPDATE USERS
        SET Balance = Balance - ?
        WHERE UserID = ? AND Balance >= ?;
    )";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "DeductBalance Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_int(STMT, 1, Balance);
    sqlite3_bind_int64(STMT, 2, UserID);
    sqlite3_bind_int(STMT, 3, Balance);
    bool Result = (sqlite3_step(STMT) == SQLITE_DONE);
    sqlite3_finalize(STMT);
    return Result;
}

bool StyxSQLite::Signin(long long UserID) {
    if (!ExecuteCommand("BEGIN TRANSACTION;")) return false;

    const std::time_t now = std::time(nullptr);
    const std::tm tm_struct = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");
    const std::string time_str = oss.str();

    LOG.Log(LoggingSystem::INFO, "Today's Date: " + time_str);

    const char* SQL =
        "INSERT OR IGNORE INTO SignIn(UserID, SignDate, Timestamp)\n"
        "VALUES(?, ?, strftime('%s','now'));";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "Signin Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        ExecuteCommand("ROLLBACK;");
        return false;
    }
    sqlite3_bind_int64(STMT, 1, UserID);
    sqlite3_bind_text(STMT, 2, time_str.c_str(), -1, SQLITE_TRANSIENT);

    bool inserted = (sqlite3_step(STMT) == SQLITE_DONE && sqlite3_changes(SQLiteDB) == 1);
    sqlite3_finalize(STMT);

    if (inserted) {
        bool ok = AddBalance(UserID, 0);
        ExecuteCommand(ok ? "COMMIT;" : "ROLLBACK;");
        return ok;
    } else {
        ExecuteCommand("ROLLBACK;");
        return false;
    }
}

int StyxSQLite::CheckBalance(long long UserID)  {
    const char* SQL = "SELECT Balance FROM USERS WHERE UserID = ?;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "CheckBalance Preparation Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return -1;
    }
    sqlite3_bind_int64(STMT, 1, UserID);

    int Balance = -1;
    if (sqlite3_step(STMT) == SQLITE_ROW) {
        Balance = sqlite3_column_int(STMT, 0);
    }
    sqlite3_finalize(STMT);
    return Balance;
}

bool StyxSQLite::SetInvite(long long UserID, long long InviteID) {
    return PrepareAndExecute("UPDATE USERS SET InviteID = ? WHERE UserID = ?;",
                             { std::to_string(InviteID), std::to_string(UserID) });
}

long long StyxSQLite::GetInviteID(long long UserID) const {
    const char* SQL = "SELECT InviteID FROM USERS WHERE UserID = ?;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int64(STMT, 1, UserID);

    long long Invite = 0;
    if (sqlite3_step(STMT) == SQLITE_ROW && sqlite3_column_type(STMT, 0) != SQLITE_NULL) {
        Invite = sqlite3_column_int64(STMT, 0);
    }
    sqlite3_finalize(STMT);
    return Invite;
}

int StyxSQLite::GetInviteNumberUsers(long long UserID) const {
    const char* SQL = "SELECT COUNT(*) FROM USERS WHERE InviteID = ?;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) return 0;
    sqlite3_bind_int64(STMT, 1, UserID);

    sqlite3_int64 Count = 0;
    if (sqlite3_step(STMT) == SQLITE_ROW && sqlite3_column_type(STMT, 0) != SQLITE_NULL) {
        Count = sqlite3_column_int64(STMT, 0);
    }
    sqlite3_finalize(STMT);

    if (Count > std::numeric_limits<int>::max()) {
        return -1;
    }
    return static_cast<int>(Count);
}

bool StyxSQLite::AddUserToGroup(long long UserID, long long ChatID) {
    const std::string SQL = "INSERT OR IGNORE INTO USER_GROUP(UserID, ChatID) VALUES(?, ?);";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "AddUserToGroup Prepare Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    sqlite3_bind_int64(STMT, 1, UserID);
    sqlite3_bind_int64(STMT, 2, ChatID);

    bool Result = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!Result)
        LOG.Log(LoggingSystem::ERROR, "AddUserToGroup Exec Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));

    sqlite3_finalize(STMT);
    return Result;
}

bool StyxSQLite::IsUserInGroup(long long UserID, long long ChatID) {
    const std::string SQL = "SELECT 1 FROM USER_GROUP WHERE UserID = ? AND ChatID = ? LIMIT 1;";
    sqlite3_stmt* STMT = nullptr;

    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "IsUserInGroup Prepare Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    sqlite3_bind_int64(STMT, 1, UserID);
    sqlite3_bind_int64(STMT, 2, ChatID);

    bool Exists = (sqlite3_step(STMT) == SQLITE_ROW);
    sqlite3_finalize(STMT);
    return Exists;
}

bool StyxSQLite::RemoveUserFromGroup(long long UserID, long long ChatID) {
    const std::string SQL = "DELETE FROM USER_GROUP WHERE UserID = ? AND ChatID = ?;";
    sqlite3_stmt* STMT = nullptr;

    if (sqlite3_prepare_v2(SQLiteDB, SQL.c_str(), -1, &STMT, nullptr) != SQLITE_OK) {
        LOG.Log(LoggingSystem::ERROR, "RemoveUserFromGroup Prepare Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }

    sqlite3_bind_int64(STMT, 1, UserID);
    sqlite3_bind_int64(STMT, 2, ChatID);

    bool Result = (sqlite3_step(STMT) == SQLITE_DONE);
    if (!Result)
        LOG.Log(LoggingSystem::ERROR, "RemoveUserFromGroup Exec Failed: " + std::string(sqlite3_errmsg(SQLiteDB)));

    sqlite3_finalize(STMT);
    return Result;
}

bool StyxSQLite::AddGroup(long long ChatID) {
    return PrepareAndExecute("INSERT OR IGNORE INTO GROUPS(ChatID) VALUES(?);",
                             { std::to_string(ChatID) });
}

bool StyxSQLite::RemoveGroup(long long ChatID) {
    return PrepareAndExecute("DELETE FROM GROUPS WHERE ChatID = ?;",
                             { std::to_string(ChatID) });
}

bool StyxSQLite::IsGroup(long long ChatID) const
{
    sqlite3_stmt* STMT = nullptr;
    const char* SQL = "SELECT 1 FROM GROUPS WHERE ChatID=? LIMIT 1;";
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int64(STMT, 1, ChatID);
    bool Found = (sqlite3_step(STMT) == SQLITE_ROW);
    sqlite3_finalize(STMT);
    return Found;
}

std::vector<long long> StyxSQLite::ListGroup() const
{
    sqlite3_stmt* STMT = nullptr;
    std::vector<long long> vec;
    const char* SQL = "SELECT ChatID FROM GROUPS;";
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) == SQLITE_OK) {
        while (sqlite3_step(STMT) == SQLITE_ROW) {
            vec.push_back(sqlite3_column_int64(STMT, 0));
        }
    }
    sqlite3_finalize(STMT);
    return vec;
}

bool StyxSQLite::GetUserIDFromUserName(const std::string& UserName, long long& OutUserID) const
{
    const char* SQL = "SELECT UserID FROM USERS WHERE FromUserName = ? LIMIT 1;";
    sqlite3_stmt* STMT = nullptr;
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(STMT, 1, UserName.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = false;
    if (sqlite3_step(STMT) == SQLITE_ROW) {
        OutUserID = sqlite3_column_int64(STMT, 0);
        ok = true;
    }
    sqlite3_finalize(STMT);
    return ok;
}

StyxSQLite::~StyxSQLite()
{
    if (SQLiteDB) {
        sqlite3_close(SQLiteDB);
        SQLiteDB = nullptr;
    }
}