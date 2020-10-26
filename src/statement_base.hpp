//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "config.hpp"
#include "handles.hpp"
#include "sql_function_wrappers.hpp"

namespace aodbc
{
    struct statement;
    struct result_set;

    struct statement_base
    {
        friend struct statement;
        friend struct result_set;

        statement_base(handles::dbc_handle &dbc)
        : stmt_(dbc)
        {
        }

        auto get_max_rows() -> SQLULEN { return sql_get_max_rows(stmt_.get_handle()); }
        auto set_max_rows(SQLULEN max_rows) -> void { sql_set_max_rows(stmt_.get_handle(), max_rows); }

        auto get_query_timeout() -> SQLULEN { return sql_get_query_timeout(stmt_.get_handle()); }
        auto set_query_timeout(SQLULEN seconds) -> void { sql_set_query_timeout(stmt_.get_handle(), seconds); }

      protected:
        handles::stmt_handle &get_stmt() { return stmt_; }
        auto                  free_stmt_close() -> void { sql_free_stmt(stmt_.get_handle(), SQL_CLOSE); }

      private:
        handles::stmt_handle stmt_;
    };
}   // namespace aodbc
