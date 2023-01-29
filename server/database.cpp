#include <string>
#include <sqlite3.h>

#include "./include/database.hpp"

server::db::DB::DB(std::string userDB)
{
    int rc = sqlite3_open(userDB.c_str(), &this->_db);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to open database: "
                                 << sqlite3_errmsg(this->_db);
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Opened database: "
                                << userDB;
    }


    char *errMsg = nullptr;
    rc = sqlite3_exec(this->_db,
                      "CREATE TABLE IF NOT EXISTS users "
                      "(nickname TEXT PRIMARY KEY, password TEXT NOT NULL);",
                      nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to create table: "
                                 << errMsg;
        sqlite3_free(errMsg);
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Created users table";
    }

    /* */
    rc = sqlite3_prepare_v2(this->_db,
                            "SELECT * FROM users WHERE nickname = ?;",
                            -1, &this->stmtUserExists, nullptr);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to generate userExists statement";
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Generated userExists statement";
    }

    /* */
    rc = sqlite3_prepare_v2(this->_db,
                            "INSERT INTO users VALUES (?, ?);",
                            -1, &this->stmtAddUser, nullptr);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to generate addUser statement";
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Generated addUser statement";
    }
}

bool
server::db::DB::userExist(std::string nick)
{
    int rc;

    rc = sqlite3_bind_text(this->stmtUserExists, 1, nick.c_str(), -1,
                           SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to check if user exists";
        return false;
    }

    rc = sqlite3_step(this->stmtUserExists);

    if (rc == SQLITE_ROW)
    {
        int count = sqlite3_column_int(this->stmtUserExists, 0);
        sqlite3_reset(this->stmtUserExists);
        return (count > 0);
    }
    else
    {
        return false;
    }
}

server::db::DB::~DB()
{
    sqlite3_finalize(this->stmtUserExists);
    sqlite3_finalize(this->stmtAddUser);
    sqlite3_close(this->_db);
}