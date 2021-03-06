//
// Created by ahodges on 07/10/2020.
//

#pragma once

#include <AODBC/config.hpp>
#include <AODBC/log.hpp>
#include <AODBC/sql_function_wrappers.hpp>

#include <memory>

namespace aodbc::handles
{
    struct impl_type
    {
        impl_type(SQL_handle_type type, std::shared_ptr< impl_type > parent)
        : handle_(SQL_NULL_HANDLE)
        , type_(type)
        , parent_(std::move(parent))
        , children_dead_(false)
        {
            if (parent_)
            {
                sql_alloc_parent(handle_, type_, &parent_->handle_);
                switch (type)
                {
                case SQL_HANDLE_ENV:
                    assert(handle_ != SQL_NULL_HENV);
                    break;
                case SQL_HANDLE_DBC:
                    assert(handle_ != SQL_NULL_HDBC);
                    break;
                case SQL_HANDLE_STMT:
                    assert(handle_ != SQL_NULL_HSTMT);
                    break;
                default:
                    std::runtime_error("Invalid type of handle. 002");
                }
            }
            else
            {
                sql_alloc(handle_, type_);
                switch (type)
                {
                case SQL_HANDLE_ENV:
                    assert(handle_ != SQL_NULL_HENV);
                    break;
                case SQL_HANDLE_DBC:
                    assert(handle_ != SQL_NULL_HDBC);
                    break;
                case SQL_HANDLE_STMT:
                    assert(handle_ != SQL_NULL_HSTMT);
                    break;
                default:
                    std::runtime_error("Invalid type of handle. 003");
                }
            }
        }

        /// @brief Check to see if the children of this object are dead or not.
        bool children_dead() const
        {
            if (parent_)
                return children_dead_ or parent_->children_dead();
            else
                return children_dead_;
        };

        void mark_children_dead() { children_dead_ = true; }

        // TODO stop this ever throwing, which it does when deallocating the child when the parent has already been
        // deallocated
        ~impl_type()
        {
            if (parent_)
            {
                if (parent_->children_dead())
                {
                    return;
                }
            }
            sql_dealloc(handle_, type_);
        }

        SQLHANDLE                    handle_;
        SQL_handle_type              type_;
        std::shared_ptr< impl_type > parent_;
        bool                         children_dead_;

        // copy and move
        impl_type(const impl_type &other) = delete;   // Copy
        impl_type &operator=(const impl_type &other) = delete;
        impl_type(impl_type &&other)                 = delete;   // Move
        impl_type &operator=(impl_type &&other) = delete;
    };

    struct base_handle
    {
        SQLHANDLE &                   get_handle() { return impl_->handle_; }
        SQL_handle_type               get_type() { return impl_->type_; }
        std::shared_ptr< impl_type > &get_parent() { return impl_->parent_; }
        std::shared_ptr< impl_type > &get_self() { return impl_; }

      protected:
        base_handle(SQL_handle_type type, std::shared_ptr< impl_type > parent)
        : impl_(std::make_shared< impl_type >(type, std::move(parent)))
        {
        }

      private:
        std::shared_ptr< impl_type > impl_;
    };

    struct env_handle : base_handle
    {
        env_handle()
        : base_handle(SQL_HANDLE_ENV, nullptr)
        {
        }
    };

    struct dbc_handle : base_handle
    {
        dbc_handle(env_handle &parent)
        : base_handle(SQL_HANDLE_DBC, parent.get_self())
        {
        }
    };

    struct stmt_handle : base_handle
    {
        stmt_handle(dbc_handle &parent)
        : base_handle(SQL_HANDLE_STMT, parent.get_self())
        {
        }
    };

}   // namespace aodbc::handles