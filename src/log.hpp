//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include <boost/log/trivial.hpp>

namespace aodbc
{
    inline void log_trace(const char *msg) { BOOST_LOG_TRIVIAL(trace) << msg; }
    inline void log_debug(const char *msg) { BOOST_LOG_TRIVIAL(debug) << msg; }
    inline void log_info(const char *msg) { BOOST_LOG_TRIVIAL(info) << msg; }
    inline void log_warning(const char *msg) { BOOST_LOG_TRIVIAL(warning) << msg; }
    inline void log_error(const char *msg) { BOOST_LOG_TRIVIAL(error) << msg; }
    inline void log_fatal(const char *msg) { BOOST_LOG_TRIVIAL(fatal) << msg; }

}   // namespace aodbc
