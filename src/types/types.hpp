//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include "sql.h"
#include "sqlext.h"
#include "types/date.hpp"
#include "types/decimal.hpp"
#include "types/nullable.hpp"
#include "types/time.hpp"
#include "types/timestamp.hpp"

#include <iostream>
#include <tuple>
#include <variant>
#include <vector>

namespace aodbc::types
{
    typedef nullable< bool > aodbc_bit;

    typedef nullable< std::int64_t > aodbc_bigint;
    // typedef std::int64_t  aodbc_sbigint;
    // typedef std::uint64_t aodbc_ubigint;

    typedef nullable< std::int32_t > aodbc_int;
    // typedef std::int32_t  aodbc_sint;
    // typedef std::uint32_t aodbc_uint;

    typedef nullable< std::int16_t > aodbc_short;
    // typedef std::int16_t  aodbc_sshort;
    // typedef std::uint16_t aodbc_ushort;

    typedef nullable< std::uint8_t > aodbc_tinyint;
    // typedef std::int8_t  aodbc_stinyint;
    // typedef std::uint8_t aodbc_utinyint;

    typedef nullable< float >  aodbc_float;
    typedef nullable< double > aodbc_double;

    typedef nullable< std::string >    aodbc_varchar;
    typedef nullable< std::u16string > aodbc_nvarchar;

    typedef nullable< std::vector< char > > aodbc_binary;   // binary type

    typedef nullable< types::date >      aodbc_date;
    typedef nullable< types::time >      aodbc_time;
    typedef nullable< types::timestamp > aodbc_timestamp;

    typedef nullable< types::decimal > aodbc_decimal;

    inline std::tuple< std::size_t, std::size_t > sqltype_to_c_size_and_alignment(int sqltype)
    {
        switch (sqltype)
        {
        case SQL_BIT:
            return std::make_tuple(sizeof(aodbc_bit), alignof(aodbc_bit));
        case SQL_BIGINT:
            return std::make_tuple(sizeof(aodbc_bigint), alignof(aodbc_bigint));
        case SQL_INTEGER:
            return std::make_tuple(sizeof(aodbc_int), alignof(aodbc_int));
        case SQL_SMALLINT:
            return std::make_tuple(sizeof(aodbc_short), alignof(aodbc_short));
        case SQL_TINYINT:
            return std::make_tuple(sizeof(aodbc_tinyint), alignof(aodbc_tinyint));
        case SQL_REAL:   // TODO FLOAD AND DOUBLE
            return std::make_tuple(sizeof(aodbc_float), alignof(aodbc_float));
        case SQL_DOUBLE:
            return std::make_tuple(sizeof(aodbc_double), alignof(aodbc_double));
        case SQL_FLOAT:
            return std::make_tuple(sizeof(aodbc_double), alignof(aodbc_double));
        case SQL_CHAR:
            return std::make_tuple(sizeof(aodbc_varchar), alignof(aodbc_varchar));
        case SQL_VARCHAR:
            return std::make_tuple(sizeof(aodbc_varchar), alignof(aodbc_varchar));
        case SQL_WCHAR:
            return std::make_tuple(sizeof(aodbc_nvarchar), alignof(aodbc_nvarchar));
        case SQL_BINARY:
            return std::make_tuple(sizeof(aodbc_binary), alignof(aodbc_binary));
        case SQL_TYPE_DATE:
            return std::make_tuple(sizeof(aodbc_date), alignof(aodbc_date));
        case -154:   // SQL_TYPE_TIME incorrect?
            return std::make_tuple(sizeof(aodbc_time), alignof(aodbc_time));
        case SQL_TYPE_TIMESTAMP:
            return std::make_tuple(sizeof(aodbc_timestamp), alignof(aodbc_timestamp));
        case SQL_DECIMAL:
            return std::make_tuple(sizeof(aodbc_decimal), alignof(aodbc_decimal));
        default:
            throw std::runtime_error("invalid type in sqltype_to_c_alignment.");
        }
    }

    aodbc_bit get_bit(handles::stmt_handle &stmt, std::size_t column_index)
    {
        bool   tmp;
        SQLLEN ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_BIT, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_bit();
        return aodbc_bit(tmp);
    }
    aodbc_bigint get_bigint(handles::stmt_handle &stmt, std::size_t column_index)
    {
        std::int64_t tmp;
        SQLLEN       ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_SBIGINT, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_bigint();
        return aodbc_bigint(tmp);
    }
    aodbc_int get_int(handles::stmt_handle &stmt, std::size_t column_index)
    {
        std::int32_t tmp;
        SQLLEN       ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_SLONG, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_int();
        return aodbc_int(tmp);
    }
    aodbc_short get_short(handles::stmt_handle &stmt, std::size_t column_index)
    {
        std::int16_t tmp;
        SQLLEN       ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_SHORT, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_short();
        return aodbc_short(tmp);
    }
    aodbc_tinyint get_tinyint(handles::stmt_handle &stmt, std::size_t column_index)
    {
        std::int8_t tmp;
        SQLLEN      ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_TINYINT, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_tinyint();
        return aodbc_tinyint(tmp);
    }
    aodbc_float get_float(handles::stmt_handle &stmt, std::size_t column_index)
    {
        float  tmp;
        SQLLEN ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_FLOAT, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_float();
        return aodbc_float(tmp);
    }
    aodbc_double get_double(handles::stmt_handle &stmt, std::size_t column_index)
    {
        double tmp;
        SQLLEN ind;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_DOUBLE, &tmp, sizeof(tmp), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_double();
        return aodbc_double(tmp);
    }
    aodbc_varchar get_varchar(handles::stmt_handle &stmt, std::size_t column_index)
    {
        SQLRETURN retcode;
        SQLLEN    ind;
        char      dummy;

        // TODO this call results in a data truncation notification
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_CHAR, &dummy, sizeof(dummy), &ind));

        if (ind == SQL_NULL_DATA)
            return aodbc_varchar();
        if (ind == 0)
            return aodbc_varchar("");

        auto tmp_str = std::string();
        if (ind == SQL_NO_TOTAL)
        {
            char buffer[1024];
            for (;;)
            {
                retcode = SQLGetData(stmt.get_handle(), column_index, SQL_C_CHAR, buffer, sizeof(buffer), &ind);
                handle_odbc_call(stmt.get_handle(), SQL_HANDLE_STMT, retcode);
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
            handle_odbc_call(
                stmt.get_handle(),
                SQL_HANDLE_STMT,
                SQLGetData(stmt.get_handle(), column_index, SQL_C_CHAR, &tmp_str[0], tmp_str.size(), &ind));
            tmp_str.resize(ind);
        }

        return aodbc_varchar(std::move(tmp_str));
    }
    aodbc_nvarchar get_nvarchar(handles::stmt_handle &, std::size_t)
    {
        throw std::runtime_error("get_nvarchar is not implemented");
    }
    aodbc_binary get_binary(handles::stmt_handle &stmt, std::size_t column_index)
    {
        SQLRETURN retcode;
        SQLLEN    ind;
        char      dummy;
        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(), column_index, SQL_C_BINARY, &dummy, 0, &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_binary();
        if (ind == 0)
            return aodbc_binary(std::vector< char >());

        std::vector< char > tmp_bin;
        if (ind == SQL_NO_TOTAL)
        {
            char buffer[1024];
            for (;;)
            {
                retcode = SQLGetData(stmt.get_handle(), column_index, SQL_C_BINARY, buffer, sizeof(buffer), &ind);
                handle_diagnostic(stmt.get_handle(), SQL_HANDLE_STMT, retcode);
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
            handle_odbc_call(
                stmt.get_handle(),
                SQL_HANDLE_STMT,
                SQLGetData(stmt.get_handle(), column_index, SQL_C_BINARY, tmp_bin.data(), tmp_bin.size(), &ind));
        }
        return aodbc_binary(std::move(tmp_bin));
    }
    aodbc_date get_date(handles::stmt_handle &stmt, std::size_t column_index)
    {
        DATE_STRUCT date_struct;
        SQLLEN      ind;

        handle_odbc_call(
            stmt.get_handle(),
            SQL_HANDLE_STMT,
            SQLGetData(stmt.get_handle(), column_index, SQL_C_DATE, &date_struct, sizeof(date_struct), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_date();
        return aodbc_date(types::date(date_struct.year, date_struct.month, date_struct.day));
    }
    aodbc_time get_time(handles::stmt_handle &stmt, std::size_t column_index)
    {
        TIME_STRUCT time_struct;
        SQLLEN      ind;
        handle_odbc_call(
            stmt.get_handle(),
            SQL_HANDLE_STMT,
            SQLGetData(stmt.get_handle(), column_index, SQL_C_TIME, &time_struct, sizeof(time_struct), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_time();
        return aodbc_time(types::time(time_struct.hour, time_struct.minute, time_struct.second));
    }
    aodbc_timestamp get_timestamp(handles::stmt_handle &stmt, std::size_t column_index)
    {
        TIMESTAMP_STRUCT timestamp_struct;
        SQLLEN           ind;
        handle_odbc_call(
            stmt.get_handle(),
            SQL_HANDLE_STMT,
            SQLGetData(
                stmt.get_handle(), column_index, SQL_C_TIMESTAMP, &timestamp_struct, sizeof(timestamp_struct), &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_timestamp();

        return aodbc_timestamp(types::timestamp(timestamp_struct.year,
                                                timestamp_struct.month,
                                                timestamp_struct.day,
                                                timestamp_struct.hour,
                                                timestamp_struct.minute,
                                                timestamp_struct.second,
                                                timestamp_struct.fraction));
    }
    aodbc_decimal get_decimal(handles::stmt_handle &stmt, std::size_t column_index)
    {
        types::decimal numeric_struct = {};
        SQLLEN         ind;

        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLGetData(stmt.get_handle(),
                                    column_index,
                                    SQL_C_NUMERIC,
                                    numeric_struct.get_impl(),
                                    sizeof(*numeric_struct.get_impl()),
                                    &ind));
        if (ind == SQL_NULL_DATA)
            return aodbc_decimal();
        return aodbc_decimal(std::move(numeric_struct));
    }

    /// C++ data type TO ODBC type identifier
    template < typename T >
    constexpr SQLSMALLINT type_to_sql_ident()
    {
        // BOOLEAN
        if constexpr (std::is_same_v< T, aodbc_bit >)
            return SQL_BIT;   // bool
        // BIGINT
        else if constexpr (std::is_same_v< T, aodbc_bigint >)
            return SQL_BIGINT;   // signed bigint
        // INT
        else if constexpr (std::is_same_v< T, aodbc_int >)
            return SQL_INTEGER;   // signed int

        // SHORT
        else if constexpr (std::is_same_v< T, aodbc_short >)
            return SQL_SMALLINT;   // signed short

        // BYTE
        else if constexpr (std::is_same_v< T, aodbc_tinyint >)
            return SQL_TINYINT;   // signed byte

        // FLOAT/DOUBLE
        else if constexpr (std::is_same_v< T, aodbc_float >)
            return SQL_REAL;   // float
        else if constexpr (std::is_same_v< T, aodbc_double >)
            return SQL_FLOAT;   // double

        // DECIMAL
        else if constexpr (std::is_same_v< T, aodbc_decimal >)
            return SQL_DECIMAL;   // decimal

        // STRING
        else if constexpr (std::is_same_v< T, aodbc_varchar >)
            return SQL_VARCHAR;   // string
        else if constexpr (std::is_same_v< T, aodbc_nvarchar >)
            return SQL_WVARCHAR;   // Nstring

        // BINARY
        else if constexpr (std::is_same_v< T, aodbc_binary >)
            return SQL_BINARY;   // binary

        // DATE/TIME/TIMESTAMP
        else if constexpr (std::is_same_v< T, aodbc_date >)
            return SQL_TYPE_DATE;   // date
        else if constexpr (std::is_same_v< T, aodbc_time >)
            return -154;   // time
        else if constexpr (std::is_same_v< T, aodbc_timestamp >)
            return SQL_TYPE_TIMESTAMP;   // timestamp
        else
            throw "invalid type";
    }

}   // namespace aodbc::types
