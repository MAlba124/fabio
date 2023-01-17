#ifndef _LOG_HPP
#define _LOG_HPP 1

#include <boost/log/core/core.hpp>
#include <string>

namespace servLog
{
    namespace logging = boost::log;

    void init(const std::string& logFile);
}

#endif //_LOG_HPP
