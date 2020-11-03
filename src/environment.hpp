//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "config.hpp"
#include "handles.hpp"
#include "log.hpp"
#include "sql_function_wrappers.hpp"

namespace aodbc
{
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

        [[nodiscard]] handles::env_handle &get_env() { return env_; }
        [[nodiscard]] bool                 initialised() const { return initialised_; }

      private:
        bool                initialised_;
        handles::env_handle env_;
    };

    inline environment &get_default_env()
    {
        static environment env;
        if (not env.initialised())
        {
            env.init();
        }
        return env;
    }

}   // namespace aodbc