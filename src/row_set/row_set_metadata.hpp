//
// Created by ahodges on 06/10/2020.
//

#pragma once

#include "config.hpp"
#include "handles.hpp"
#include "sql_function_wrappers.hpp"
#include "types/types.hpp"
#include <typeinfo>

namespace aodbc::row_set
{
    struct column_metadata
    {
        friend struct row_set_metadata;

        column_metadata(SQLUSMALLINT column_number)
        : number_(column_number)
        {
        }

        friend void generate(column_metadata &metadata, handles::stmt_handle &stmt);
        [[nodiscard]] const std::string & name() const {return name_;}
        [[nodiscard]] std::tuple<std::size_t,std::size_t> size_and_alignment() const {return null_types::sqltype_to_c_size_and_alignment(sql_type);}



      private:
        SQLUSMALLINT number_;    // Column number
        std::string  name_;       // Column name
        SQLSMALLINT  sql_type;   // SQL type
        std::size_t  byte_len;   // Size of column
        SQLSMALLINT  decimal;    // decimal digits / scale
        SQLSMALLINT  nullable;   // column nullability
    };

    struct row_set_metadata
    {
        row_set_metadata() {}

        void build_metadata(handles::stmt_handle &stmt);

        [[nodiscard]] std::size_t            column_count() const { return column_metadata_.size(); }
        [[nodiscard]] const column_metadata &get_column_metadata(std::size_t column) const
        {
            return column_metadata_[column];
        }

      private:
        std::vector< column_metadata > column_metadata_;
    };

    SQLSMALLINT get_column_count(handles::stmt_handle &stmt);

    // row_set_metadata build_metadata(SQLHSTMT &handle_stmt_);

}   // namespace aodbc::row_set