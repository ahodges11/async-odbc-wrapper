//
// Created by ahodges on 27/10/2020.
//

#pragma once

#include <AODBC/handles.hpp>
#include <AODBC/sync/result_set/metadata.hpp>
#include <AODBC/sync/result_set/result_set_concept.hpp>

namespace aodbc::sync::result_set
{
    struct standard_result_set final : result_set_concept
    {
        standard_result_set(handles::stmt_handle &stmt)
        : stmt_(stmt)
        {
        }

        void init() override
        {
            // Build column metadata
            metadata_.generate_column_metadata(stmt_);
            metadata_.genrate_key_column_index();
        }

        // Iterate the cursor to the next rowset
        bool next(std::vector<message> * messages= nullptr)
        {
            SQLRETURN rc = SQLFetch(stmt_.get_handle());
            if (rc == SQL_NO_DATA)
                return false;
            handle_diagnostic(stmt_.get_handle(), SQL_HANDLE_STMT, messages);
            return true;
        }

        types::aodbc_bit      get_bit(std::size_t column_index) { return types::get_bit(stmt_, column_index); };
        types::aodbc_bigint   get_bigint(std::size_t column_index) { return types::get_bigint(stmt_, column_index); };
        types::aodbc_int      get_int(std::size_t column_index) { return types::get_int(stmt_, column_index); };
        types::aodbc_short    get_short(std::size_t column_index) { return types::get_short(stmt_, column_index); };
        types::aodbc_tinyint  get_tinyint(std::size_t column_index) { return types::get_tinyint(stmt_, column_index); };
        types::aodbc_float    get_float(std::size_t column_index) { return types::get_float(stmt_, column_index); };
        types::aodbc_double   get_double(std::size_t column_index) { return types::get_double(stmt_, column_index); };
        types::aodbc_varchar  get_varchar(std::size_t column_index)
        {

            auto str_len = metadata_.get_col(column_index).byte_len() + 1;
            if (str_len != 1)
                return types::get_varchar(stmt_, column_index, str_len);
            else
                return types::get_varchar(stmt_, column_index);

        };
        types::aodbc_nvarchar get_nvarchar(std::size_t column_index)
        {
            return types::get_nvarchar(stmt_, column_index);
        };
        types::aodbc_binary    get_binary(std::size_t column_index) { return types::get_binary(stmt_, column_index); };
        types::aodbc_date      get_date(std::size_t column_index) { return types::get_date(stmt_, column_index); };
        types::aodbc_time      get_time(std::size_t column_index) { return types::get_time(stmt_, column_index); };
        types::aodbc_timestamp get_timestamp(std::size_t column_index)
        {
            return types::get_timestamp(stmt_, column_index);
        };
        types::aodbc_decimal get_decimal(std::size_t column_index) { return types::get_decimal(stmt_, column_index); };

        template < typename DesiredType >
        bool column_is_type(std::size_t column_index)
        {
            return metadata_.column_is_type(column_index, types::type_to_sql_ident< DesiredType >());
        }

        std::size_t key_to_column_index(const std::string &key) { return metadata_.key_to_column_index(key); }
        const sync::result_set::result_set_metadata &get_metadata() { return metadata_; }

      private:
        handles::stmt_handle stmt_;
        result_set_metadata  metadata_;
    };

}   // namespace aodbc::sync::result_set