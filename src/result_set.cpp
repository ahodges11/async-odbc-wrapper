//
// Created by ahodges on 06/10/2020.
//

#include "result_set.hpp"

#include "sql_function_wrappers.hpp"

#define return_if_null(ind)                                                                                            \
    if (ind == SQL_NULL_DATA)                                                                                          \
    {                                                                                                                  \
        return result;                                                                                                 \
    }

namespace aodbc
{
    template < typename value_type >
    types::types_variant result_set::get(std::uint16_t column_index)
    {
        auto       result = types::types_variant();
        value_type tmp;
        SQLLEN     ind;
        handle_odbc_call(
            stmt_.get_handle(),
            SQL_HANDLE_STMT,
            SQLGetData(
                stmt_.get_handle(), column_index, types::type_to_sql_ident< value_type >(), &tmp, sizeof(tmp), &ind));
        return_if_null(ind);
        result.emplace< value_type >(tmp);
        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_decimal >(std::uint16_t column_index)
    {
        auto                 result         = types::types_variant();
        types::aodbc_decimal numeric_struct = {};
        SQLLEN               ind;

        handle_odbc_call(stmt_.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt_.get_handle(),
                                    column_index,
                                    types::type_to_sql_ident< types::aodbc_decimal >(),
                                    numeric_struct.get_impl(),
                                    sizeof(*numeric_struct.get_impl()),
                                    &ind));
        return_if_null(ind);
        result.emplace< types::aodbc_decimal >(std::move(numeric_struct));
        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_date >(std::uint16_t column_index)
    {
        auto        result = types::types_variant();
        DATE_STRUCT date_struct;
        SQLLEN      ind;

        handle_odbc_call(stmt_.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt_.get_handle(),
                                    column_index,
                                    types::type_to_sql_ident< types::aodbc_date >(),
                                    &date_struct,
                                    sizeof(date_struct),
                                    &ind));
        return_if_null(ind);
        result.emplace< types::aodbc_date >(date_struct.year, date_struct.month, date_struct.day);
        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_time >(std::uint16_t column_index)
    {
        auto        result = types::types_variant();
        TIME_STRUCT time_struct;
        SQLLEN      ind;
        handle_odbc_call(stmt_.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt_.get_handle(),
                                    column_index,
                                    types::type_to_sql_ident< types::aodbc_time >(),
                                    &time_struct,
                                    sizeof(time_struct),
                                    &ind));
        return_if_null(ind);
        result.emplace< types::aodbc_time >(time_struct.hour, time_struct.minute, time_struct.second);
        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_timestamp >(std::uint16_t column_index)
    {
        auto             result = types::types_variant();
        TIMESTAMP_STRUCT timestamp_struct;
        SQLLEN           ind;
        handle_odbc_call(stmt_.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt_.get_handle(),
                                    column_index,
                                    types::type_to_sql_ident< types::aodbc_timestamp >(),
                                    &timestamp_struct,
                                    sizeof(timestamp_struct),
                                    &ind));
        return_if_null(ind);
        result.emplace< types::aodbc_timestamp >(timestamp_struct.year,
                                                 timestamp_struct.month,
                                                 timestamp_struct.day,
                                                 timestamp_struct.hour,
                                                 timestamp_struct.minute,
                                                 timestamp_struct.second,
                                                 timestamp_struct.fraction);
        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_varchar >(std::uint16_t column_index)
    {
        auto      result = types::types_variant();
        SQLRETURN retcode;
        SQLLEN    ind;
        char      dummy;

        // TODO this call results in a data truncation notification
        handle_odbc_call(stmt_.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt_.get_handle(),
                                    column_index,
                                    types::type_to_sql_ident< types::aodbc_varchar >(),
                                    &dummy,
                                    sizeof(dummy),
                                    &ind));
        return_if_null(ind);
        if (ind == 0)
        {
            result.emplace< types::aodbc_varchar >("");
            return result;
        }

        auto &tmp_str = result.emplace< types::aodbc_varchar >();
        if (ind == SQL_NO_TOTAL)
        {
            char buffer[1024];
            for (;;)
            {
                retcode = SQLGetData(stmt_.get_handle(),
                                     column_index,
                                     types::type_to_sql_ident< types::aodbc_varchar >(),
                                     buffer,
                                     sizeof(buffer),
                                     &ind);
                handle_odbc_call(stmt_.get_handle(), SQL_HANDLE_STMT, retcode);
                std::size_t len =
                    (ind == SQL_NO_TOTAL) ? sizeof(buffer) - 1 : std::min< std::size_t >(sizeof(buffer) - 1, ind);
                tmp_str.append(buffer, len);
                if (retcode == SQL_SUCCESS)
                    break;
            }
        }
        else
        {
            tmp_str.resize(ind + 1);
            handle_odbc_call(stmt_.get_handle(),
                             SQL_HANDLE_STMT,
                             SQLGetData(stmt_.get_handle(),
                                        column_index,
                                        types::type_to_sql_ident< types::aodbc_varchar >(),
                                        &tmp_str[0],
                                        tmp_str.size(),
                                        &ind));
            tmp_str.resize(ind);
        }

        return result;
    }

    template <>
    types::types_variant result_set::get< types::aodbc_binary >(std::uint16_t column_index)
    {
        auto      result = types::types_variant();
        SQLRETURN retcode;
        SQLLEN    ind;
        char      dummy;
        handle_odbc_call(
            stmt_.get_handle(),
            SQL_HANDLE_STMT,
            SQLGetData(
                stmt_.get_handle(), column_index, types::type_to_sql_ident< types::aodbc_binary >(), &dummy, 0, &ind));
        return_if_null(ind);
        if (ind == 0)
        {
            result.emplace< types::aodbc_binary >();
            return result;
        }

        auto &tmp_bin = result.emplace< types::aodbc_binary >();
        if (ind == SQL_NO_TOTAL)
        {
            char buffer[1024];
            for (;;)
            {
                retcode = SQLGetData(stmt_.get_handle(),
                                     column_index,
                                     types::type_to_sql_ident< types::aodbc_binary >(),
                                     buffer,
                                     sizeof(buffer),
                                     &ind);
                handle_diagnostic(stmt_.get_handle(), SQL_HANDLE_STMT, retcode);
                if (ind == SQL_NO_TOTAL)
                    tmp_bin.insert(tmp_bin.end(), buffer, buffer + sizeof(buffer));
                else
                    tmp_bin.insert(tmp_bin.end(), buffer, buffer + ind);
                if (retcode == SQL_SUCCESS)
                    break;
            }
        }
        else
        {
            tmp_bin.resize(ind);
            handle_odbc_call(stmt_.get_handle(),
                             SQL_HANDLE_STMT,
                             SQLGetData(stmt_.get_handle(),
                                        column_index,
                                        types::type_to_sql_ident< types::aodbc_binary >(),
                                        tmp_bin.data(),
                                        tmp_bin.size(),
                                        &ind));
        }
        return result;
    }

    /*
    template <>   // TODO
    types::types_variant get< types::aodbc_nvarchar >(std::uint16_t column_index)
    {
        auto result = types::types_variant();
        return result;
    }
    */

    // implicit generation declarations for the linker
    template types::types_variant result_set::get< types::aodbc_bit >(std::uint16_t);

    template types::types_variant result_set::get< types::aodbc_sbigint >(std::uint16_t);
    template types::types_variant result_set::get< types::aodbc_ubigint >(std::uint16_t);

    template types::types_variant result_set::get< types::aodbc_sint >(std::uint16_t);
    template types::types_variant result_set::get< types::aodbc_uint >(std::uint16_t);

    template types::types_variant result_set::get< types::aodbc_sshort >(std::uint16_t);
    template types::types_variant result_set::get< types::aodbc_ushort >(std::uint16_t);

    template types::types_variant result_set::get< types::aodbc_stinyint >(std::uint16_t);
    template types::types_variant result_set::get< types::aodbc_utinyint >(std::uint16_t);

    template types::types_variant result_set::get< types::aodbc_float >(std::uint16_t);
    template types::types_variant result_set::get< types::aodbc_double >(std::uint16_t);

}   // namespace aodbc
