//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "config.hpp"
#include "handles.hpp"
#include "log.hpp"
#include "sql_function_wrappers.hpp"
#include "connection.hpp"

namespace aodbc
{
    // forward declare
    struct environment;
    namespace detail
    {
        environment &get_singleton();
    }

    // define
    struct environment
    {
        environment() noexcept
        : initialised_(false)
        {
        }

        void init()
        {
            // Set env attributes
            sql_set_env_attr(env_.get_handle(), SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            initialised_ = true;
        }

        connection create_connection(std::string connection_str)
        {
            return connection(get_env(),std::move(connection_str));
        }

        [[nodiscard]] handles::env_handle &get_env() { return env_; }
        [[nodiscard]] bool                 initialised() const { return initialised_; }

      private:
        bool                initialised_;
        handles::env_handle env_;

    };


}   // namespace aodbc