//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "config.hpp"
#include "log.hpp"
#include "sql_function_wrappers.hpp"

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
        , handle_env(nullptr)
        {
        }

        static environment &get_singleton() { return detail::get_singleton(); }

        bool init()
        {
            if (valid())
            {
                // log that user is trying to init env twice
                log_warning("WARNING: attempting to init the environment twice");
                return false;
            }

            // Allocate handle_env
            sql_alloc_env(&handle_env);

            // Set env attributes
            sql_set_env_attr(&handle_env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
            initialised_ = true;
            return true;
        }

        ~environment()
        {
            if (valid())
            {
                sql_dealloc_env(&handle_env);
            }
        }

        [[nodiscard]] bool     valid() const { return handle_env; }
        [[nodiscard]] SQLHENV *get_env()
        {
            assert(initialised_);
            return &handle_env;
        }

      private:
        bool    initialised_;
        SQLHENV handle_env;

    } singleton = {};

    namespace detail
    {
        environment &get_singleton() { return singleton; }
    }   // namespace detail

}   // namespace aodbc