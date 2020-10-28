//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include "date.hpp"
#include "time.hpp"

namespace aodbc::types
{
    struct timestamp
    : date
    , time
    {
        timestamp()
        : timestamp(0, 1, 1, 0, 0, 0, 0)
        {
        }

        timestamp(std::int16_t  year,
                  std::uint16_t month,
                  std::uint16_t day,
                  std::uint16_t hour,
                  std::uint16_t minute,
                  std::uint16_t second,
                  std::uint32_t fraction)
        : date(year, month, day)
        , time(hour, minute, second)
        , fraction_(fraction)
        {
        }

        timestamp(const date &datep, const time &timep, std::uint32_t fraction)
        : date(datep)
        , time(timep)
        , fraction_(fraction)
        {
        }

        std::uint32_t fraction() const { return fraction_; }

        bool operator==(const timestamp &other) const
        {
            return date::operator==(other) && time::operator==(other) && fraction_ == other.fraction_;
        }

      private:
        std::uint32_t fraction_;
    };
}   // namespace aodbc::types
