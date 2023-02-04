#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>

#include "./include/log.hpp"

static void consoleFormatter(
        boost::log::record_view const& rec,
        boost::log::formatting_ostream& strm)
{
    auto severity = rec[boost::log::trivial::severity];
    if (severity)
    {
        strm << "[ ";
        switch (severity.get())
        {
            case boost::log::trivial::severity_level::trace:
                strm << "\033[36mTRACE";
                break;
            case boost::log::trivial::severity_level::debug:
                strm << "\033[36mDEBUG";
                break;
            case boost::log::trivial::severity_level::info:
                strm << "\033[34mINFO";
                break;
            case boost::log::trivial::severity_level::warning:
                strm << "\033[33mWARNING";
                break;
            case boost::log::trivial::severity_level::error:
                strm << "\033[31mERROR";
                break;
            case boost::log::trivial::severity_level::fatal:
                strm << "\033[43m\033[31mFATAL";
                break;
            default:
                break;
        }

        strm << "\033[0m ] :: ";
    }

    strm << rec[boost::log::expressions::smessage];
}

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

    boost::shared_ptr<
            logging::sinks::synchronous_sink<
            logging::sinks::basic_text_ostream_backend<char>
            >> consoleSink = logging::add_console_log();

    consoleSink->set_formatter(&consoleFormatter);

    logging::add_common_attributes();
}