//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include <AODBC/handles.hpp>
#include <AODBC/sql_function_wrappers.hpp>

#include <memory>

namespace aodbc
{
    struct statement
    {
        statement(handles::dbc_handle &dbc)
        : stmt_(dbc)
        {
        }

        // Default attributes
        static const std::size_t DEFAULT_TIMEOUT  = 0;
        static const std::size_t DEFAULT_MAX_ROWS = 0;

        /// @brief Executes the provided sql_statement
        /// @param sql_statement The sql_statement
        /// @return Quantity of affected rows, -1 if none.
        long execute(std::string &sql_statement, std::vector<message> * messages=nullptr)
        {
            // execute the query
            sql_exec_direct(stmt_.get_handle(), sql_statement, messages);

            // get effected rows
            SQLLEN row_count;
            sql_row_count(stmt_.get_handle(), &row_count);
            return row_count;
        }

        template < typename ResultSet >
        std::unique_ptr< ResultSet > execute_query(std::string &           sql_statement,
                                                   std::vector< message > *messages = nullptr)
        {
            free_stmt_close();   // Close any existing cursor
            sql_exec_direct(stmt_.get_handle(), sql_statement, messages);
            auto result_set = std::make_unique< ResultSet >(stmt_);
            result_set->init();
            return result_set;
        }

      public:   // query statement state
        auto get_max_rows() -> SQLULEN { return sql_get_max_rows(stmt_.get_handle()); }
        auto get_query_timeout() -> SQLULEN { return sql_get_query_timeout(stmt_.get_handle()); }

      public:   // set statement state
        auto set_max_rows(SQLULEN max_rows) -> void { sql_set_max_rows(stmt_.get_handle(), max_rows); }
        auto set_query_timeout(SQLULEN seconds) -> void { sql_set_query_timeout(stmt_.get_handle(), seconds); }

      private:
        handles::stmt_handle &get_stmt() { return stmt_; }
        auto                  free_stmt_close() -> void { sql_free_stmt(stmt_.get_handle(), SQL_CLOSE); }

      private:   // data members
        handles::stmt_handle stmt_;
    };

}   // namespace aodbc
