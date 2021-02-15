//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <AODBC/types/date.hpp>
#include <AODBC/types/time.hpp>

#include <type_traits>
#include <sqltypes.h>
#include <tuple>

namespace aodbc::types
{
    struct timestamp : SQL_TIMESTAMP_STRUCT
    {
        timestamp(std::int16_t  year     = 0,
                  std::uint16_t month    = 1,
                  std::uint16_t day      = 1,
                  std::uint16_t hour     = 0,
                  std::uint16_t minute   = 0,
                  std::uint16_t second   = 0,
                  std::uint32_t fraction = 0)
        : SQL_TIMESTAMP_STRUCT { .year     = year,
                                 .month    = month,
                                 .day      = day,
                                 .hour     = hour,
                                 .minute   = minute,
                                 .second   = second,
                                 .fraction = fraction }
        {
        }

        bool operator==(const timestamp &other) const
        {
            return std::tie(year, month, day, hour, minute, second, fraction) ==
                   std::tie(other.year, other.month, other.day, other.hour, other.minute, other.second, other.fraction);
        }
    };

    static_assert(std::is_standard_layout_v<timestamp>);
}   // namespace aodbc::types
