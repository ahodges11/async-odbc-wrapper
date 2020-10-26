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
#include <tuple>

#include <iostream>
#include <variant>
#include <vector>

namespace aodbc::null_types
{
    typedef types::nullable<bool> aodbc_bit;

    typedef types::nullable< std::int64_t > aodbc_bigint;
    // typedef std::int64_t  aodbc_sbigint;
    // typedef std::uint64_t aodbc_ubigint;

    typedef types::nullable< std::int32_t > aodbc_int;
    // typedef std::int32_t  aodbc_sint;
    // typedef std::uint32_t aodbc_uint;

    typedef types::nullable< std::int16_t > aodbc_short;
    // typedef std::int16_t  aodbc_sshort;
    // typedef std::uint16_t aodbc_ushort;

    typedef types::nullable< std::uint8_t > aodbc_tinyint;
    // typedef std::int8_t  aodbc_stinyint;
    // typedef std::uint8_t aodbc_utinyint;

    typedef types::nullable< float > aodbc_float;
    typedef double                   aodbc_double;

    typedef types::nullable< std::string >    aodbc_varchar;
    typedef types::nullable< std::u16string > aodbc_nvarchar;

    typedef types::nullable< std::vector< char > > aodbc_binary;   // binary type

    typedef types::nullable< types::date >      aodbc_date;
    typedef types::nullable< types::time >      aodbc_time;
    typedef types::nullable< types::timestamp > aodbc_timestamp;

    typedef types::nullable< types::decimal > aodbc_decimal;

    inline std::tuple<std::size_t,std::size_t> sqltype_to_c_size_and_alignment(int sqltype)
    {
        switch (sqltype)
        {
        case SQL_BIT:
            return std::make_tuple(sizeof(aodbc_bit),alignof(aodbc_bit));
        case SQL_BIGINT:
            return std::make_tuple(sizeof(aodbc_bigint),alignof(aodbc_bigint));
        case SQL_INTEGER:
            return std::make_tuple(sizeof(aodbc_int),alignof(aodbc_int));
        case SQL_SMALLINT:
            return std::make_tuple(sizeof(aodbc_short),alignof(aodbc_short));
        case SQL_TINYINT:
            return std::make_tuple(sizeof(aodbc_tinyint),alignof(aodbc_tinyint));
        case SQL_REAL: //TODO FLOAD AND DOUBLE
            return std::make_tuple(sizeof(aodbc_float),alignof(aodbc_float));
        case SQL_DOUBLE:
            return std::make_tuple(sizeof(aodbc_double),alignof(aodbc_double));
        case SQL_FLOAT:
            return std::make_tuple(sizeof(aodbc_double),alignof(aodbc_double));
        case SQL_CHAR:
            return std::make_tuple(sizeof(aodbc_varchar),alignof(aodbc_varchar));
        case SQL_VARCHAR:
            return std::make_tuple(sizeof(aodbc_varchar),alignof(aodbc_varchar));
        case SQL_WCHAR:
            return std::make_tuple(sizeof(aodbc_nvarchar),alignof(aodbc_nvarchar));
        case SQL_BINARY:
            return std::make_tuple(sizeof(aodbc_binary),alignof(aodbc_binary));
        case SQL_TYPE_DATE:
            return std::make_tuple(sizeof(aodbc_date),alignof(aodbc_date));
        case -154: // SQL_TYPE_TIME incorrect?
            return std::make_tuple(sizeof(aodbc_time),alignof(aodbc_time));
        case SQL_TYPE_TIMESTAMP:
            return std::make_tuple(sizeof(aodbc_timestamp),alignof(aodbc_timestamp));
        case SQL_DECIMAL:
            return std::make_tuple(sizeof(aodbc_decimal),alignof(aodbc_decimal));
        default:
            throw std::runtime_error("invalid type in sqltype_to_c_alignment.");
        }
    }

}   // namespace aodbc::null_types

namespace aodbc::types
{
    typedef bool aodbc_bit;

    typedef std::int64_t  aodbc_bigint;
    typedef std::int64_t  aodbc_sbigint;
    typedef std::uint64_t aodbc_ubigint;

    typedef std::int32_t  aodbc_int;
    typedef std::int32_t  aodbc_sint;
    typedef std::uint32_t aodbc_uint;

    typedef std::int16_t  aodbc_short;
    typedef std::int16_t  aodbc_sshort;
    typedef std::uint16_t aodbc_ushort;

    typedef std::uint8_t aodbc_tinyint;
    typedef std::int8_t  aodbc_stinyint;
    typedef std::uint8_t aodbc_utinyint;

    typedef float  aodbc_float;
    typedef double aodbc_double;

    typedef std::string    aodbc_varchar;
    typedef std::u16string aodbc_nvarchar;

    typedef std::vector< char > aodbc_binary;   // binary type

    typedef date      aodbc_date;
    typedef time      aodbc_time;
    typedef timestamp aodbc_timestamp;

    typedef decimal aodbc_decimal;

    // null
    struct aodbc_null
    {
    };

    /// C++ data type TO ODBC type identifier
    template < typename T >
    constexpr SQLSMALLINT type_to_sql_ident()
    {
        // BOOLEAN
        if constexpr (std::is_same_v< T, aodbc_bit >)
            return SQL_C_BIT;   // bool

        // BIGINT
        else if constexpr (std::is_same_v< T, aodbc_sbigint >)
            return SQL_C_SBIGINT;   // signed bigint
        else if constexpr (std::is_same_v< T, aodbc_ubigint >)
            return SQL_C_UBIGINT;   // unsigned bigint

        // INT
        else if constexpr (std::is_same_v< T, aodbc_sint >)
            return SQL_C_SLONG;   // signed int
        else if constexpr (std::is_same_v< T, aodbc_uint >)
            return SQL_C_ULONG;   // unsigned int

        // SHORT
        else if constexpr (std::is_same_v< T, aodbc_sshort >)
            return SQL_C_SHORT;   // signed short
        else if constexpr (std::is_same_v< T, aodbc_ushort >)
            return SQL_C_USHORT;   // unsigned short

        // BYTE
        else if constexpr (std::is_same_v< T, aodbc_stinyint >)
            return SQL_C_TINYINT;   // signed byte
        else if constexpr (std::is_same_v< T, aodbc_utinyint >)
            return SQL_C_UTINYINT;   // unsigned byte

        // FLOAT/DOUBLE
        else if constexpr (std::is_same_v< T, aodbc_float >)
            return SQL_C_FLOAT;   // float
        else if constexpr (std::is_same_v< T, aodbc_double >)
            return SQL_C_DOUBLE;   // double

        // DECIMAL
        else if constexpr (std::is_same_v< T, aodbc_decimal >)
            return SQL_C_NUMERIC;   // decimal

        // STRING
        else if constexpr (std::is_same_v< T, aodbc_varchar >)
            return SQL_C_CHAR;   // string
        else if constexpr (std::is_same_v< T, aodbc_nvarchar >)
            return SQL_C_WCHAR;   // Nstring

        // BINARY
        else if constexpr (std::is_same_v< T, aodbc_binary >)
            return SQL_C_BINARY;   // binary

        // DATE/TIME/TIMESTAMP
        else if constexpr (std::is_same_v< T, aodbc_date >)
            return SQL_C_DATE;   // date
        else if constexpr (std::is_same_v< T, aodbc_time >)
            return SQL_C_TIME;   // time
        else if constexpr (std::is_same_v< T, aodbc_timestamp >)
            return SQL_C_TIMESTAMP;   // timestamp

        else
            throw "invalid type";
    }

    using types_variant = std::variant< aodbc_null,
                                        aodbc_bit,
                                        aodbc_sbigint,
                                        aodbc_ubigint,
                                        aodbc_sint,
                                        aodbc_uint,
                                        aodbc_sshort,
                                        aodbc_ushort,
                                        aodbc_stinyint,
                                        aodbc_utinyint,
                                        aodbc_varchar,
                                        aodbc_nvarchar,
                                        aodbc_float,
                                        aodbc_double,
                                        aodbc_date,
                                        aodbc_time,
                                        aodbc_timestamp,
                                        aodbc_decimal,
                                        aodbc_binary >;

}   // namespace aodbc::types
