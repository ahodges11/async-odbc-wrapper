//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include "statement_base.hpp"
#include <optional>

namespace aodbc
{

    struct result_set
    {
        result_set(statement_base & stmt_base)
        :stmt_base_(stmt_base)
        {
        }

        /// @brief Return the result set metadata including the column information and row count.
        void fetch_metadata()
        {}

        std::optional<row> fetch_one()
        {
            if (next())
            {
                // build and return row
            }
            return std::nullopt;
        }

        void fetch_many()
        {}

        void fetch_all()
        {}

        void close()
        {
            stmt_base_.free_stmt_close();
        }

      private:
        // Iterate the cursor to the next rowset
        bool next()
        {
            SQLRETURN rc = SQLFetch(*stmt_base_.get_handle());
            if (rc == SQL_NO_DATA)
                return false;
            handle_diagnostic(*stmt_base_.get_handle(),SQL_HANDLE_STMT,rc);
            return true;
        }



      private:
        statement_base & stmt_base_;
    };

}