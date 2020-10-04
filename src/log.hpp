//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include <boost/log/trivial.hpp>

namespace aodbc
{
    void log_trace(const char *msg) { BOOST_LOG_TRIVIAL(trace) << msg; }
    void log_debug(const char *msg) { BOOST_LOG_TRIVIAL(debug) << msg; }
    void log_info(const char *msg) { BOOST_LOG_TRIVIAL(info) << msg; }
    void log_warning(const char *msg) { BOOST_LOG_TRIVIAL(warning) << msg; }
    void log_error(const char *msg) { BOOST_LOG_TRIVIAL(error) << msg; }
    void log_fatal(const char *msg) { BOOST_LOG_TRIVIAL(fatal) << msg; }

}   // namespace aodbc
