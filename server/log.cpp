#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include "./include/log.hpp"

// TODO: Make the logger flush the file more often
void
servLog::init(const std::string& logFile)
{
    logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");

    logging::add_file_log
    (
            logging::keywords::file_name = "%N_" + logFile,
            logging::keywords::rotation_size = 25 * 1024 * 1024,
            logging::keywords::format = "[%TimeStamp%] [%Severity%] :: %Message%"
    );

    logging::core::get()->set_filter
    (
            logging::trivial::severity >= logging::trivial::info
    );

    logging::add_common_attributes();
}