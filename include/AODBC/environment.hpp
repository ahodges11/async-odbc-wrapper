//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include <AODBC/config.hpp>
#include <AODBC/handles.hpp>
#include <AODBC/log.hpp>
#include <AODBC/sql_function_wrappers.hpp>

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
        static auto env = [](){
            auto env = environment();
            env.init();
            return env;
        }();
        assert(env.initialised());
        return env;
    }



}   // namespace aodbc