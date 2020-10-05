//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "sql_function_wrappers.hpp"
#include "statement_base.hpp"
#include "result_set.hpp"

namespace aodbc
{
    struct connection;

    struct statement : statement_base
    {
        friend struct connection;

        statement()
        : statement_base()
        {
        }


        /// @brief Executes the provided sql_statement
        /// @param sql_statement The sql_statement
        /// @return Quantity of affected rows, -1 if none.
        SQLLEN execute(std::string &sql_statement)
        {
            assert(valid());
            // execute the query
            sql_exec_direct(&handle_stmt_, sql_statement);

            // get effected rows
            SQLLEN row_count;
            sql_row_count(&handle_stmt_,&row_count);
            return row_count;
        }

        /// @brief Executes the provided sql_statement as a query, returning the results
        /// @param sql_statement The sql_statement
        /// @return Returns a result_set containing the data produced by the query
        result_set execute_query(std::string &sql_statement)
        {
            auto result = result_set();
            free_stmt_close(); // Close any existing cursor
            sql_exec_direct(&handle_stmt_,sql_statement);

        }

    };

}   // namespace aodbc
