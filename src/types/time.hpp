//
// Created by ahodges on 05/10/2020.
//

#pragma once
#include <cstdint>
#include <sqltypes.h>

#include <tuple>

namespace aodbc::types
{
    struct time : SQL_TIME_STRUCT
    {
        time(std::uint16_t hour = 0, std::uint16_t minute = 0, std::uint16_t second = 0)
        : SQL_TIME_STRUCT { .hour = hour, .minute = minute, .second = second }
        {
        }

        bool operator==(const time &other) const
        {
            return std::tie(hour,minute,second) == std::tie(other.hour,other.minute,other.second);
        }
    };

    static_assert(std::is_standard_layout_v< time >);
}   // namespace aodbc::types
