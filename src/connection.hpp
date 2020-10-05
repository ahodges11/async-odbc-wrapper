//
// Created by ahodges on 03/10/2020.
//

#pragma once

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
        connection(SQLHENV *handle_env, std::string connection_str)
        : connection_str_(std::move(connection_str))
        , connected_(false)
        , handle_env_(handle_env)
        , handle_dbc_(SQL_NULL_HANDLE)
        {
            // allocate a connection
            sql_alloc_dbc(handle_env_, &handle_dbc_);
        }

        void connect()
        {
            // connect to the driver
            sql_driver_connect(&handle_dbc_, connection_str_);
            log_info("Connected driver");
            connected_ = true;
        }

        void disconnect()
        {
            assert(connected());
            sql_driver_disconnect(&handle_dbc_);
            connected_ = false;
        }

        statement create_statement()
        {
            auto stmt = statement();
            sql_alloc_stmt(&handle_dbc_,stmt.get_handle());
            return stmt;
        }


        bool connected() const { return connected_; }
        bool valid() const { return handle_dbc_; }

        ~connection()
        {
            if (connected_)
            {
                log_warning("connection destroyed while it was still connected.");
                disconnect();
            }
            if (valid())
            {
                sql_dealloc_dbc(&handle_dbc_);
            }
        }

      private:
        std::string connection_str_;
        bool        connected_;
        SQLHENV *   handle_env_;   // ptr to the connection's environment handle
        SQLHDBC     handle_dbc_;   // the underlying odbc connection handle
    };

}   // namespace aodbc