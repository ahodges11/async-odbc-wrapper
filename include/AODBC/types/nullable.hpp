//
// Created by ahodges on 05/10/2020.
//

#pragma once

#include <utility>

namespace aodbc::types
{
    template < typename T >
    struct nullable
    {
        using type = T;

      public:   // constructors
        nullable()
        : val_()
        , null_(true)
        {
        }

        nullable(const T &val)
        : val_(val)
        , null_(false)
        {
        }

        nullable(T &&val)
        : val_(std::move(val))
        , null_(false)
        {
        }

        nullable(const nullable< T > &other)
        : val_(other.val_)
        , null_(other.null_)
        {
        }

        nullable(nullable< T > &&other)
        : val_(std::move(other.val_))
        , null_(other.null_)
        {
        }

        nullable< T > &operator=(const nullable< T > &other)
        {
            val_  = other.val_;
            null_ = other.null_;
            return *this;
        }

        nullable< T > &operator=(nullable< T > &&other) noexcept
        {
            val_  = std::move(other.val_);
            null_ = other.null_;
            return *this;
        }

      public:
        bool null() const { return null_; }

        const T &operator*() const { return val_; }
        T &      operator*() { return val_; }

        const T *operator->() const { return &val_; }
        T *      operator->() { return &val_; }

      private:
        T    val_;
        bool null_;
    };
}   // namespace aodbc::types
