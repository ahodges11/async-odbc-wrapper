//
// Created by ahodges on 24/11/2020.
//

#pragma once

#include <AODBC/sync/result_set/compact_result_set.hpp>

namespace aodbc::async::result_set
{



    namespace detail
    {
        struct impl
        {
            impl(std::unique_ptr< sync::result_set::compact_result_set > impl, net::any_io_executor exec)
            : exec_(exec)
            , impl_(std::move(impl))
            {
            }

            net::awaitable< bool > fetch_one()
            {
                co_return co_await net::co_spawn(
                    exec_,
                    [&]() mutable -> net::awaitable< bool > { co_return impl_->fetch_one(); },
                    net::use_awaitable);
            }
            net::awaitable< bool > fetch_many(std::size_t quantity)
            {
                co_return co_await net::co_spawn(
                    exec_,
                    [&]() mutable -> net::awaitable< bool > { co_return impl_->fetch_many(quantity); },
                    net::use_awaitable);
            }
            net::awaitable< void > fetch_all()
            {
                co_await net::co_spawn(
                    exec_,
                    [&]() mutable -> net::awaitable< void > { co_return impl_->fetch_all(); },
                    net::use_awaitable);
            }
            net::awaitable< void > fetch_all(std::size_t rows_per_fetch)
            {
                co_await net::co_spawn(
                    exec_,
                    [&]() mutable -> net::awaitable< void > { co_return impl_->fetch_all(rows_per_fetch); },
                    net::use_awaitable);
            }

            sync::result_set::result_view get_result_view() const {return impl_->get_result_view();}

            std::shared_ptr< const sync::result_set::compact_metadata > get_metadata() { return impl_->get_metadata(); }

          private:
            net::any_io_executor                                    exec_;
            std::unique_ptr< sync::result_set::compact_result_set > impl_;
        };
    }   // namespace detail

    struct compact_result_set
    {
        typedef sync::result_set::compact_result_set impl_type;
        compact_result_set(std::unique_ptr< sync::result_set::compact_result_set > impl, net::any_io_executor exec)
        : impl_(std::move(impl), exec)
        {
        }

        net::awaitable< bool > fetch_one() { co_return co_await impl_.fetch_one(); }
        net::awaitable< bool > fetch_many(std::size_t quantity) { co_return co_await impl_.fetch_many(quantity); }
        net::awaitable< void > fetch_all() { co_await impl_.fetch_all(); }
        net::awaitable< void > fetch_all(std::size_t rows_per_fetch) { co_await impl_.fetch_all(rows_per_fetch); }

        sync::result_set::result_view get_result_view() const {return impl_.get_result_view();}

        // metadata is threadsafe after `init()` is called on the underlying sync resultset,
        // which happens before the user has access to this object.
        std::shared_ptr< const sync::result_set::compact_metadata > get_metadata() { return impl_.get_metadata(); }

      private:
        detail::impl impl_;
    };
}   // namespace aodbc::async::result_set