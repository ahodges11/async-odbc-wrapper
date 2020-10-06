//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <cstdint>
#include <sql.h>
#include <memory>

namespace aodbc::types
{
    namespace detail
    {
        struct decimal_impl
        {
            unsigned char precision;
            signed char   scale;
            unsigned char sign;                       /* 1=pos 0=neg */
            unsigned char val[SQL_MAX_NUMERIC_LEN];   // 16 //
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

      private:
        detail::decimal_impl *impl_;
    };
}   // namespace aodbc::types