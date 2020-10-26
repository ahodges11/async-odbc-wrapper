//
// Created by ahodges on 03/10/2020.
//

#pragma once

#include "handles.hpp"
#include "log.hpp"
#include "sql_function_wrappers.hpp"
#include "statement.hpp"

#include <boost/core/ignore_unused.hpp>
#include <iostream>
#include <string>

namespace aodbc
{
    struct connection
    {
        connection(handles::env_handle &env, std::string connection_str)
        : connection_str_(std::move(connection_str))
        , connected_(false)
        , dbc_(env)
        {
        }

        ~connection()
        {
            if (connected_)
            {
                log_warning("connection destroyed while it was still connected.");
                disconnect();
            }
        }

        void connect()
        {
            // connect to the driver
            sql_driver_connect(dbc_.get_handle(), connection_str_);
            log_info("Connected driver");
            connected_ = true;
        }
        void disconnect()
        {
            assert(connected());
            sql_driver_disconnect(dbc_.get_handle());
            connected_ = false;
        }

        statement create_statement() { return statement(dbc_); }

        bool                 connected() const { return connected_; }
        handles::dbc_handle &get_dbc() { return dbc_; }

      private:
        std::string         connection_str_;
        bool                connected_;
        handles::dbc_handle dbc_;   // the underlying odbc connection handle
    };

}   // namespace aodbc