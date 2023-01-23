#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>

#include "./include/log.hpp"

void
servLog::init(const std::string& logFile)
{
    logging::register_simple_formatter_factory
            <logging::trivial::severity_level, char>("Severity");

    logging::add_file_log
    (
            /* Add the logfile number before logFile */
            logging::keywords::file_name = "%N_" + logFile,

            /* Rotate files every 25 MiB */
            logging::keywords::rotation_size = 25 * 1024 * 1024,

            /* The log message format */
            logging::keywords::format =
                    "[ %TimeStamp% ] [ %Severity% ] :: %Message%",

            /* Enable automatic flushing */
            logging::keywords::auto_flush = true
    );

    logging::core::get()->set_filter
    (
            logging::trivial::severity >= logging::trivial::trace
    );

    /* Output to stdout too */
    logging::add_console_log(std::cout, boost::log::keywords::format =
            "[ \033[33m%TimeStamp%\033[0m ] [ \033[36m%Severity%\033[0m ] :: %Message%");

    logging::add_common_attributes();
}