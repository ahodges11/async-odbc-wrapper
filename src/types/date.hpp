//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <cstdint>

namespace aodbc::types
{
    struct date
    {
        date()
        : date(0, 1, 1)
        {
        }

        date(std::int16_t year, std::uint16_t month, std::uint16_t day)
        : year_(year)
        , month_(month)
        , day_(day)
        {
        }

        std::int16_t year() const {return year_;}
        std::uint16_t month() const {return month_;}
        std::uint16_t day() const {return day_;}

        bool operator==(const date & other) const
        {
            if (year_ != other.year_ || month_ != other.month_ || day_ != other.day_)
                return false;
            else
                return true;
        }

      private:
        std::int16_t year_;
        std::uint16_t month_;
        std::uint16_t day_;
    };
}   // namespace aodbc::types