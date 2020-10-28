//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <cstdint>
#include <memory>
#include <sql.h>

namespace aodbc::types
{
    namespace detail
    {
        struct decimal_impl
        {
            unsigned char                                    precision;
            signed char                                      scale;
            unsigned char                                    sign;  /* 1=pos 0=neg */
            std::array< unsigned char, SQL_MAX_NUMERIC_LEN > val;   // 16 //

            bool operator==(const decimal_impl &other) const
            {
                return precision == other.precision && scale == other.scale && sign == other.sign && val == other.val;
            }
        };
    }   // namespace detail

    struct decimal
    {
      public:   // constructors
        decimal()
        : impl_(new detail::decimal_impl())
        {
        }
        ~decimal() { delete impl_; }

        // allow move operations
        decimal(decimal &&other)
        : impl_(std::exchange(other.impl_, nullptr))
        {
        }

        decimal &operator=(decimal &&other) noexcept
        {
            std::swap(impl_, other.impl_);
            return *this;
        }

        // delete copy operations
        decimal(const decimal &other) = delete;
        decimal &operator=(const decimal &other) = delete;

      public:   // interface
        detail::decimal_impl *get_impl() { return impl_; }

        bool operator==(const decimal &other) const { return *impl_ == *other.impl_; }

      private:
        detail::decimal_impl *impl_;
    };
}   // namespace aodbc::types