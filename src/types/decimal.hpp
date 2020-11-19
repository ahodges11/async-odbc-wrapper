//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include <sql.h>
#include <sqltypes.h>
#include <tuple>
#include <type_traits>

namespace aodbc::types
{
    struct decimal : SQL_NUMERIC_STRUCT
    {
        bool operator==(const decimal &other) const
        {
            return std::tie(precision, scale, sign, val) ==
                   std::tie(other.precision, other.scale, other.sign, other.val);
        }
    };

    static_assert(std::is_standard_layout_v< decimal >);
}   // namespace aodbc::types