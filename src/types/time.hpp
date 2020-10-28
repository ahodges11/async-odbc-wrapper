//
// Created by ahodges on 05/10/2020.
//

#pragma once
#include <cstdint>

namespace aodbc::types
{
    struct time
    {
        time()
        : time(0, 0, 0)
        {
        }

        time(std::uint16_t hour, std::uint16_t minute, std::uint16_t second)
        : hour_(hour)
        , minute_(minute)
        , second_(second)
        {
        }

        std::uint16_t hour() const { return hour_; }
        std::uint16_t minute() const { return minute_; }
        std::uint16_t second() const { return second_; }

        bool operator==(const time &other) const
        {
            return hour_ == other.hour_ && minute_ == other.minute_ && second_ == other.second_;
        }

      private:
        std::uint16_t hour_;
        std::uint16_t minute_;
        std::uint16_t second_;
    };
}   // namespace aodbc::types
