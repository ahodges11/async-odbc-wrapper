//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include "handles.hpp"
#include "row_set/row_set_base.hpp"
#include "row_set/row_set_metadata.hpp"
#include "types/types.hpp"

#include <boost/core/ignore_unused.hpp>
#include <optional>

namespace aodbc
{
    struct result_set
    {
        result_set(handles::stmt_handle stmt)
        : stmt_(std::move(stmt))
        {
        }

        /// @brief Determines how many rows are fetched from SQL with each Fetch call.
        /// @param rowsize The quantity of rows to fetch.
        void   set_cursor_rowsize(std::size_t rowsize) { sql_set_row_size(get_stmt().get_handle(), rowsize); }
        SQLLEN get_cursor_rowsize() { return sql_get_row_size(get_stmt().get_handle()); }

        template < typename RowSetType, typename... Args >
        RowSetType create_row_set(Args &&... args)
        {
            if (not metadata_)
            {
                metadata_ = std::make_unique< row_set::row_set_metadata >();
                metadata_->build_metadata(get_stmt());
            }

            auto result = RowSetType(*metadata_, std::forward(args)...);
            return result;
        }

        // Iterate the cursor to the next rowset
        bool next()
        {
            SQLRETURN rc = SQLFetch(stmt_.get_handle());
            if (rc == SQL_NO_DATA)
                return false;
            handle_diagnostic(stmt_.get_handle(), SQL_HANDLE_STMT, rc);
            return true;
        }

        /// @brief Get the data object at the specified column.
        /// @tparam value_type The aodbc type of the field, e.g. types::aodbc_bigint
        /// @param column_index The column index to the requested field
        /// @return type_variant containing the specified type OR aodbc_null of the field was a null value.
        ///
        /// Note, if the given `value_type` doesn't correspond with the actual column type of the result_set it's
        /// undefined behaviour.
        template < typename value_type >
        types::types_variant get(std::uint16_t column_index);

        handles::stmt_handle &get_stmt() { return stmt_; }

      private:
        handles::stmt_handle                         stmt_;
        std::unique_ptr< row_set::row_set_metadata > metadata_;
    };

}   // namespace aodbc