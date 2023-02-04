// TODO: write doc

#ifndef _DATABASE_HPP
#define _DATABASE_HPP 1

#include <boost/log/trivial.hpp>
#include <memory>
#include <string>
#include <mutex>

#include <sqlite3.h>

namespace server::db
{
    struct User {
        bool err;
        std::string nick;
        int balance;
    };

    class DB
        : std::enable_shared_from_this<DB>
    {
    protected:
        sqlite3 *_db{};
        sqlite3_stmt *stmtUserExists{};
        sqlite3_stmt *stmtAddUser{};
        sqlite3_stmt *stmtCheck{};
        sqlite3_stmt *stmtGetUser{};
        std::mutex m;
    public:
        explicit DB(std::string usersDB);
        bool userExist(const std::string& nick);
        User userAdd(const std::string& nick, const std::string& pass,
                     int bal);
        User userValidate(const std::string& nick, const std::string& pass);
        ~DB();
    };
}

#endif //_DATABASE_HPP
