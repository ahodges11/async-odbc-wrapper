//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <cstdint>
#include <sqltypes.h>
#include <tuple>
#include <type_traits>

namespace aodbc::types
{
    struct date : SQL_DATE_STRUCT
    {
        date(std::int16_t year = 0, std::uint16_t month = 1, std::uint16_t day = 1)
        : SQL_DATE_STRUCT { .year = year, .month = month, .day = day }
        {
        }

        bool operator==(const date &other) const
        {
            return std::tie(year, month, day) == std::tie(other.year, other.month, other.day);
        }
    };

    static_assert(std::is_standard_layout_v< date >);
}   // namespace aodbc::types