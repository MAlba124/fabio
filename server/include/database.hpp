// TODO: write doc

#ifndef _DATABASE_HPP
#define _DATABASE_HPP 1

#include <boost/log/trivial.hpp>
#include <memory>
#include <string>
#include <sqlite3.h>

namespace server::db
{
    class DB
        : std::enable_shared_from_this<DB>
    {
    protected:
        sqlite3 *_db{};
        sqlite3_stmt *stmtUserExists{};
        sqlite3_stmt *stmtAddUser{};
    public:
        explicit DB(std::string usersDB);
        bool userExist(std::string nick);
        ~DB();
    };
}

#endif //_DATABASE_HPP
