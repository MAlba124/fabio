#include <string>
#include <mutex>

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
                      "(nickname TEXT PRIMARY KEY, "
                      "password TEXT NOT NULL, "
                      "balance INTEGER);",
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
                            "INSERT INTO users VALUES (?, ?, ?);",
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

    /* */
    rc = sqlite3_prepare_v2(this->_db,
                            "SELECT password FROM users WHERE nickname = ?;",
                            -1, &this->stmtCheck, nullptr);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to generate check statement";
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Generated check statement";
    }

    /* */
    rc = sqlite3_prepare_v2(this->_db,
                            "SELECT balance FROM users WHERE nickname = ?;",
                            -1, &this->stmtGetUser, nullptr);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to generate getUser statement";
        return;
    }
    else
    {
        BOOST_LOG_TRIVIAL(info) << "Generated getUser statement";
    }
}

bool
server::db::DB::userExist(const std::string& nick)
{
    std::lock_guard<std::mutex> lock(this->m);

    int rc;

    rc = sqlite3_bind_text(this->stmtUserExists, 1, nick.c_str(), -1,
                           SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(warning) << "Failed to check if user exists: "
                                   << sqlite3_errmsg(this->_db);
        return false;
    }

    rc = sqlite3_step(this->stmtUserExists);

    if (rc == SQLITE_ROW)
    {
        //int count = sqlite3_column_int(this->stmtUserExists, 0); // ?? (TODO)
        sqlite3_reset(this->stmtUserExists);
        return true;
    }
    else
    {
        return false;
    }
}

server::db::User
server::db::DB::userAdd(const std::string& nick, const std::string& pass,
                        int bal)
{
    std::lock_guard<std::mutex> lock(this->m);

    int rc;
    server::db::User u = { .err = true, .nick = nick, .balance = bal };

    rc = sqlite3_bind_text(this->stmtAddUser, 1, nick.c_str(), -1,
                           SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to bind_text(1)";
        return u;
    }

    rc = sqlite3_bind_text(this->stmtAddUser, 2, pass.c_str(), -1,
                           SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to bind_text(2)";
        return u;
    }

    rc = sqlite3_bind_int(this->stmtAddUser, 3, bal);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to bind_int";
        return u;
    }

    rc = sqlite3_step(this->stmtAddUser);
    if (rc != SQLITE_DONE)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to step";
        return u;
    }

    sqlite3_reset(this->stmtAddUser);

    u.err = false;

    return u;
}

server::db::User
server::db::DB::userValidate(const std::string& nick, const std::string& pass)
{
    std::lock_guard<std::mutex> lock(this->m);

    int rc;
    server::db::User u = { .err = true, .nick = nick, .balance = -1 };

    rc = sqlite3_bind_text(this->stmtCheck, 1, nick.c_str(), -1,
                           SQLITE_STATIC);
    if (rc != SQLITE_OK)
    {
        BOOST_LOG_TRIVIAL(debug) << "Failed to bind_text";
        return u;
    }

    rc = sqlite3_step(this->stmtCheck);
    if (rc == SQLITE_ROW)
    {
        const char *password = reinterpret_cast<const char*>
                (sqlite3_column_text(this->stmtCheck, 0));

        if (pass == password)
        {
            sqlite3_reset(this->stmtCheck);

            rc = sqlite3_bind_text(this->stmtGetUser, 1, nick.c_str(), -1,
                                   SQLITE_STATIC);
            if (rc != SQLITE_OK)
            {
                sqlite3_reset(this->stmtGetUser);
                return u;
            }

            if (sqlite3_step(this->stmtGetUser) == SQLITE_ROW)
            {
                u.balance = sqlite3_column_int(this->stmtGetUser, 0);
            }
            else {
                sqlite3_reset(this->stmtGetUser);
                return u;
            }

            sqlite3_reset(this->stmtGetUser);
            u.err = false;

            return u;
        }
    }

    sqlite3_reset(this->stmtCheck);

    return u;
}

server::db::DB::~DB()
{
    sqlite3_finalize(this->stmtUserExists);
    sqlite3_finalize(this->stmtAddUser);
    sqlite3_finalize(this->stmtCheck);
    sqlite3_finalize(this->stmtGetUser);
    sqlite3_close(this->_db);
}