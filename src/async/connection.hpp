//
// Created by ahodges on 30/10/2020.
//

#pragma once

#include "async/result_set/result_set_concept.hpp"
#include "sync.hpp"

#include <boost/function.hpp>
#include <boost/thread/concurrent_queues/sync_queue.hpp>
#include <threads.h>

namespace aodbc::async
{
    namespace detail
    {

        struct connection_wrapper
        {
            connection_wrapper()
            : thread_pool_(std::make_shared<net::thread_pool>(1))
            {
            }

            net::awaitable< void > connect(std::string &str)
            {
                co_await net::co_spawn(
                    thread_pool_->get_executor(),
                    [&]() -> net::awaitable< void > { co_return connection_.connect(str); },
                    net::use_awaitable);
            }
            net::awaitable< void > disconnect()
            {
                co_await net::co_spawn(
                    thread_pool_->get_executor(),
                    [&]() -> net::awaitable< void > { co_return connection_.disconnect(); },
                    net::use_awaitable);
            }

            net::awaitable< long >
            execute(std::string &sql_statement, std::size_t timeout = 0, std::size_t max_rows = 0)
            {
                co_return co_await net::co_spawn(
                    thread_pool_->get_executor(),
                    [&]() -> net::awaitable< long > {
                        co_return connection_.execute(sql_statement, timeout, max_rows);
                    },
                    net::use_awaitable);
            }

            template < result_set::AsyncResultSet ResultSet >
            net::awaitable< std::unique_ptr< ResultSet > >
            execute_query(std::string &sql_statement, std::size_t timeout = 0, std::size_t max_rows = 0)
            {
                co_return std::make_unique< ResultSet >(
                    co_await net::co_spawn(
                        thread_pool_->get_executor(),
                        [&]() -> net::awaitable< std::unique_ptr<typename ResultSet::impl_type> > {
                            co_return connection_.execute_query< typename ResultSet::impl_type >(
                                sql_statement, timeout, max_rows);
                        },
                        net::use_awaitable),
                    thread_pool_);
            }

          private:
            // thread to do work on
            std::shared_ptr< net::thread_pool > thread_pool_;
            sync::connection                    connection_;
        };
    }   // namespace detail

    struct connection
    {
        explicit connection(net::any_io_executor exec)
        : exec_(exec)
        {
        }

        connection(const connection &other) = delete;
        connection &operator=(const connection &other) = delete;

        net::awaitable< void > connect(std::string &connection_str) { co_await impl_.connect(connection_str); }

        net::awaitable< void > disconnect() { co_await impl_.disconnect(); }

      public:   // execute statements
        net::awaitable< long > execute(std::string &sql_statement, std::size_t timeout = 0, std::size_t max_rows = 0)
        {
            co_return co_await impl_.execute(sql_statement, timeout, max_rows);
        }

        template < result_set::AsyncResultSet ResultSet >
        net::awaitable< std::unique_ptr< ResultSet > >
        execute_query(std::string &sql_statement, std::size_t timeout = 0, std::size_t max_rows = 0)
        {
            co_return co_await impl_.execute_query< ResultSet >(sql_statement, timeout, max_rows);
        }

      private:
        net::any_io_executor       exec_;
        detail::connection_wrapper impl_;
    };
}   // namespace aodbc::async
