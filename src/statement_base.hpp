//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "config.hpp"

namespace aodbc
{
    struct statement;
    struct result_set;

    struct statement_base
    {
        friend struct statement;
        friend struct result_set;

        bool valid() const { return handle_stmt_ != SQL_NULL_HANDLE; }

        auto get_max_rows() -> SQLULEN { return sql_get_max_rows(&handle_stmt_); }
        auto set_max_rows(SQLULEN max_rows) -> void { sql_set_max_rows(&handle_stmt_, max_rows); }

        auto get_query_timeout() -> SQLULEN { return sql_get_query_timeout(&handle_stmt_); }
        auto set_query_timeout(SQLULEN seconds) -> void { sql_set_query_timeout(&handle_stmt_, seconds); }



      protected:
        SQLHSTMT *get_handle() { return &handle_stmt_; }
        auto free_stmt_close() -> void { sql_free_stmt(&handle_stmt_, SQL_CLOSE); }

      private:
        statement_base()
        : handle_stmt_(SQL_NULL_HANDLE)
        {
        }

        ~statement_base()
        {
            if (valid())
            {
                sql_dealloc_stmt(&handle_stmt_);
                handle_stmt_ = SQL_NULL_HANDLE;
            }
        }

      private:
        SQLHSTMT handle_stmt_;
    };
}   // namespace aodbc
