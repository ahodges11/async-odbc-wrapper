//
// Created by ahodges on 27/10/2020.
//

#pragma once

#include <AODBC/config.hpp>
#include <AODBC/handles.hpp>
#include <AODBC/types/types.hpp>

#include <AODBC/log.hpp>
#include <unordered_map>
#include <vector>

namespace aodbc::sync::result_set
{
    namespace detail
    {
        inline SQLSMALLINT get_column_count(handles::stmt_handle &stmt)
        {
            SQLSMALLINT result;
            handle_odbc_call(stmt.get_handle(), SQL_HANDLE_STMT, SQLNumResultCols(stmt.get_handle(), &result));
            return result;
        }
    }   // namespace detail

    struct column_metadata
    {
        friend struct row_set_metadata;

        column_metadata(SQLUSMALLINT column_number, handles::stmt_handle &stmt)
        : number_(column_number)
        {
            char        name_buf[1024];
            SQLSMALLINT name_len = 0;

            handle_odbc_call(stmt.get_handle(),
                             SQL_HANDLE_STMT,
                             SQLDescribeCol(stmt.get_handle(),
                                            number_,
                                            (SQLCHAR *)name_buf,
                                            sizeof(name_buf),
                                            &name_len,
                                            &sql_type,
                                            &byte_len_,
                                            &decimal,
                                            &nullable));
            name_.append(name_buf, name_len);
        }
        SQLSMALLINT to_c_type() const
        {
            return types::sql_type_to_c_type(sql_type);
        }

        SQLUSMALLINT                     number() const { return number_; }
        [[nodiscard]] const std::string &name() const { return name_; }
        std::size_t                      byte_len() const { return byte_len_; }

        [[nodiscard]] std::tuple< std::size_t, std::size_t > c_size_and_alignment() const
        {
            return types::sql_type_to_c_size_and_alignment(sql_type);
        }
        [[nodiscard]] std::tuple< std::size_t, std::size_t > cpp_size_and_alignment() const
        {
            return types::sql_type_to_cpp_size_and_alignment(sql_type);
        }
        SQLSMALLINT sql_type_ident() const { return sql_type; }

      private:
        SQLUSMALLINT number_;     // Column number
        std::string  name_;       // Column name
        SQLSMALLINT  sql_type;    // SQL type
        std::size_t  byte_len_;   // Size of column
        SQLSMALLINT  decimal;     // decimal digits / scale
        SQLSMALLINT  nullable;    // column nullability
    };

    struct result_set_metadata
    {
        void generate_column_metadata(handles::stmt_handle &stmt)
        {
            // Get the quantity of columns
            auto column_count = detail::get_column_count(stmt);
            column_metadata_.reserve(column_count);

            for (auto i = 1; i <= column_count; i++)
            {
                column_metadata_.emplace_back(i, stmt);   // Create and fill a column_metadata
            }
        }
        void genrate_key_column_index()
        {
            for (auto &col : column_metadata_)
            {
                key_column_index_.emplace(col.name(), col.number());
            }
        }

        const std::vector< column_metadata > &get_column_metadata() const { return column_metadata_; }
        const column_metadata &               get_col(std::size_t column_number) const
        {
            assert(column_number <= column_metadata_.size());
            return column_metadata_[column_number - 1];
        }

        bool column_is_type(std::size_t column_index, SQLSMALLINT c_type_ident)
        {
            assert(column_index <= column_metadata_.size());
            return column_metadata_[column_index-1].sql_type_ident() == c_type_ident;
        }

        std::size_t key_to_column_index(const std::string &key) const
        {
            auto ifind = key_column_index_.find(key);
            if (ifind == std::end(key_column_index_))
                throw std::runtime_error("key not valid in result_set");
            return ifind->second;
        }

      private:
        std::vector< column_metadata >                 column_metadata_;
        std::unordered_map< std::string, std::size_t > key_column_index_;   // column_names to column_index
    };
}   // namespace aodbc::sync::result_set