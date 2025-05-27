#include "StyxSQLite.HPP"

StyxSQLite::StyxSQLite(std::string  SQLitePath) : SQLiteDB(nullptr), SQLiteFilePath(std::move(SQLitePath)), LOG("SQLite-LOG.txt"){}

bool StyxSQLite::INIT()
{
    if (sqlite3_open(SQLiteFilePath.c_str(), &SQLiteDB) != SQLITE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "Unable to Open Database");
        return false;
    }

    sqlite3_busy_timeout(SQLiteDB, 5000);

    if (!ExecuteCommand("BEGIN TRANSACTION;")) return false;

    const std::vector<std::string> TableCreationStatement = {
        "CREATE TABLE IF NOT EXISTS ADMIN  (UserID   INTEGER PRIMARY KEY);",
        "CREATE TABLE IF NOT EXISTS USERS  (ID       INTEGER PRIMARY KEY AUTOINCREMENT, UserID INTEGER UNIQUE, FromName TEXT, FromUserName TEXT UNIQUE, Balance INTEGER DEFAULT 0, InviteID INTEGER NULL, Violations INTEGER DEFAULT 0);",
        "CREATE TABLE IF NOT EXISTS SignIn (UserID   INTEGER NOT NULL, SignDate TEXT NOT NULL, Timestamp INTEGER NOT NULL, PRIMARY KEY (UserID, SignDate));",
        "CREATE TABLE IF NOT EXISTS GROUPS (ChatID   INTEGER PRIMARY KEY);",
        "CREATE TABLE IF NOT EXISTS BUTTON (ID       INTEGER PRIMARY KEY AUTOINCREMENT, Type     TEXT NOT NULL, Title    TEXT NOT NULL, Data     TEXT NOT NULL, CommandType TEXT NOT NULL);",
        "CREATE TABLE IF NOT EXISTS ADS (ID          INTEGER PRIMARY KEY AUTOINCREMENT, Title    TEXT NOT NULL, URL      TEXT NOT NULL );"
    };

    for (const auto& STMT : TableCreationStatement) {
        if (!ExecuteCommand(STMT)) {
            if (!ExecuteCommand("ROLLBACK;")) {
                LOG.Log(LoggingSystem::ERROR, "[INIT] ROLLBACK Error" +  std::string(sqlite3_errmsg(SQLiteDB)));
                return false;
            }
            return false;
        }
    }

    if (!ExecuteCommand("COMMIT;")) {
        LOG.Log(LoggingSystem::ERROR, "[INIT] COMMIT Error" +  std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    return true;
}

bool StyxSQLite::ExecuteCommand(const std::string& SQL)
{
    char* ErrMSG = nullptr;
    int Res = sqlite3_exec(SQLiteDB, SQL.c_str(), nullptr, nullptr, &ErrMSG);
    if (Res != SQLITE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "SQL Statement Error: " + std::string(ErrMSG));
        sqlite3_free(ErrMSG);
        return false;
    }
    return true;
}

bool StyxSQLite::AddButton(const Button& Button)
{
    sqlite3_stmt* STMT = nullptr;
    const char* SQL = "INSERT INTO BUTTON (Type, Title, Data, CommandType) VALUES (?, ?, ?, ?);";
    if (sqlite3_prepare_v2(SQLiteDB, SQL, -1, &STMT, nullptr) != SQLITE_OK)
    {
        LOG.Log(LoggingSystem::ERROR, "Failed to prepare statement: " + std::string(sqlite3_errmsg(SQLiteDB)));
        return false;
    }
    sqlite3_bind_text(STMT, 1, Button.Type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 2, Button.Title.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 3, Button.Data.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(STMT, 4, Button.CommandType.c_str(), -1, SQLITE_TRANSIENT);
    if (sqlite3_step(STMT) != SQLITE_DONE)
    {
        LOG.Log(LoggingSystem::ERROR, "Failed to insert button: " + std::string(sqlite3_errmsg(SQLiteDB)));
        sqlite3_finalize(STMT);
        return false;
    }
    sqlite3_finalize(STMT);
    return true;
}


StyxSQLite::~StyxSQLite() {if (SQLiteDB) sqlite3_close(SQLiteDB);}