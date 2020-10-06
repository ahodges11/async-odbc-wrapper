//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include "types/date.hpp"
#include "types/time.hpp"
#include "types/timestamp.hpp"
#include "types/decimal.hpp"
#include "types/nullable.hpp"


#include <variant>
#include <vector>
#include <iostream>

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
                                        aodbc_binary>;



    namespace detail
    {
        void print_type_sizes()
        {
            std::cout << "aodbc_null: " << sizeof(aodbc::types::aodbc_null) << std::endl;
            std::cout << "aodbc_bit: " << sizeof(aodbc::types::aodbc_bit) << std::endl;
            std::cout << "aodbc_sbigint: "  << sizeof(aodbc::types::aodbc_sbigint) << std::endl;
            std::cout << "aodbc_ubigint: "  << sizeof(aodbc::types::aodbc_ubigint) << std::endl;
            std::cout << "aodbc_sint: "  << sizeof(aodbc::types::aodbc_sint) << std::endl;
            std::cout << "aodbc_uint: "  << sizeof(aodbc::types::aodbc_uint) << std::endl;
            std::cout << "aodbc_sshort: "  << sizeof(aodbc::types::aodbc_sshort) << std::endl;
            std::cout << "aodbc_ushort: "  << sizeof(aodbc::types::aodbc_ushort) << std::endl;
            std::cout << "aodbc_stinyint: "  << sizeof(aodbc::types::aodbc_stinyint) << std::endl;
            std::cout << "aodbc_utinyint: "  << sizeof(aodbc::types::aodbc_utinyint) << std::endl;
            std::cout << "aodbc_varchar: "  << sizeof(aodbc::types::aodbc_varchar) << std::endl;
            std::cout << "aodbc_nvarchar: "  << sizeof(aodbc::types::aodbc_nvarchar) << std::endl;
            std::cout << "aodbc_float: "  << sizeof(aodbc::types::aodbc_float) << std::endl;
            std::cout << "aodbc_double: "  << sizeof(aodbc::types::aodbc_double) << std::endl;
            std::cout << "aodbc_date: "  << sizeof(aodbc::types::aodbc_date) << std::endl;
            std::cout << "aodbc_date: "  << sizeof(aodbc::types::aodbc_date) << std::endl;
            std::cout << "aodbc_time: "  << sizeof(aodbc::types::aodbc_time) << std::endl;
            std::cout << "aodbc_timestamp: "  << sizeof(aodbc::types::aodbc_timestamp) << std::endl;
            std::cout << "aodbc_decimal: "  << sizeof(aodbc::types::aodbc_decimal) << std::endl;
            std::cout << "aodbc_binary: "  << sizeof(aodbc::types::aodbc_binary) << std::endl;
            std::cout << "types_variant: " << sizeof(aodbc::types::types_variant) << std::endl;
        }

        void print_type_alignment()
        {
            std::cout << "aodbc_null: " << alignof(aodbc::types::aodbc_null) << std::endl;
            std::cout << "aodbc_bit: " << alignof(aodbc::types::aodbc_bit) << std::endl;
            std::cout << "aodbc_sbigint: "  << alignof(aodbc::types::aodbc_sbigint) << std::endl;
            std::cout << "aodbc_ubigint: "  << alignof(aodbc::types::aodbc_ubigint) << std::endl;
            std::cout << "aodbc_sint: "  << alignof(aodbc::types::aodbc_sint) << std::endl;
            std::cout << "aodbc_uint: "  << alignof(aodbc::types::aodbc_uint) << std::endl;
            std::cout << "aodbc_sshort: "  << alignof(aodbc::types::aodbc_sshort) << std::endl;
            std::cout << "aodbc_ushort: "  << alignof(aodbc::types::aodbc_ushort) << std::endl;
            std::cout << "aodbc_stinyint: "  << alignof(aodbc::types::aodbc_stinyint) << std::endl;
            std::cout << "aodbc_utinyint: "  << alignof(aodbc::types::aodbc_utinyint) << std::endl;
            std::cout << "aodbc_varchar: "  << alignof(aodbc::types::aodbc_varchar) << std::endl;
            std::cout << "aodbc_nvarchar: "  << alignof(aodbc::types::aodbc_nvarchar) << std::endl;
            std::cout << "aodbc_float: "  << alignof(aodbc::types::aodbc_float) << std::endl;
            std::cout << "aodbc_double: "  << alignof(aodbc::types::aodbc_double) << std::endl;
            std::cout << "aodbc_date: "  << alignof(aodbc::types::aodbc_date) << std::endl;
            std::cout << "aodbc_date: "  << alignof(aodbc::types::aodbc_date) << std::endl;
            std::cout << "aodbc_time: "  << alignof(aodbc::types::aodbc_time) << std::endl;
            std::cout << "aodbc_timestamp: "  << alignof(aodbc::types::aodbc_timestamp) << std::endl;
            std::cout << "aodbc_decimal: "  << alignof(aodbc::types::aodbc_decimal) << std::endl;
            std::cout << "aodbc_binary: "  << alignof(aodbc::types::aodbc_binary) << std::endl;
            std::cout << "types_variant: " << alignof(aodbc::types::types_variant) << std::endl;
        }
    }

}   // namespace aodbc::types
