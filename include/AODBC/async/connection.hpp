//
// Created by ahodges on 30/10/2020.
//

#pragma once

#include <AODBC/async/result_set/result_set_concept.hpp>
#include <AODBC/async/thread_pool.hpp>
#include <AODBC/sync.hpp>
#include <boost/function.hpp>
#include <boost/thread/concurrent_queues/sync_queue.hpp>
#include <memory>
#include <threads.h>

namespace aodbc::async
{
    namespace detail
    {
        struct connection_wrapper : std::enable_shared_from_this< connection_wrapper >
        {
            connection_wrapper()
            : exec_(net::system_executor())
            , connection_(std::make_shared< sync::connection >())
            {
            }

            ~connection_wrapper()
            {
                if (connection_->connected())
                {
                    net::dispatch(get_executor(), [conn = connection_]() { conn->disconnect(); });
                }
            }

            net::awaitable< bool > connect(std::string connection_str, std::vector< message > *messages)
            {
                auto work = [self = shared_from_this(), str = std::move(connection_str), messages]() mutable
                    -> net::awaitable< bool > { co_return self->connection_->connect(str, messages); };
                co_return co_await net::co_spawn(get_executor(), std::move(work), net::use_awaitable);
            }
            net::awaitable< void > disconnect()
            {
                co_await net::co_spawn(
                    get_executor(),
                    [self = shared_from_this()]() -> net::awaitable< void > {
                        co_return self->connection_->disconnect();
                    },
                    net::use_awaitable);
            }

            net::awaitable< long > execute(std::string &           sql_statement,
                                           std::size_t             timeout  = 0,
                                           std::size_t             max_rows = 0,
                                           std::vector< message > *messages = nullptr)
            {
                co_return co_await net::co_spawn(
                    get_executor(),
                    [self          = shared_from_this(),
                     sql_statement = std::move(sql_statement),
                     timeout,
                     max_rows,
                     messages]() mutable -> net::awaitable< long > {
                        co_return self->connection_->execute(sql_statement, timeout, max_rows, messages);
                    },
                    net::use_awaitable);
            }

            template < typename ResultSet >
            net::awaitable< std::unique_ptr< ResultSet > > execute_query(std::string &           sql_statement,
                                                                         std::size_t             timeout  = 0,
                                                                         std::size_t             max_rows = 0,
                                                                         std::vector< message > *messages = nullptr)
            {
                co_return std::make_unique< ResultSet >(
                    co_await net::co_spawn(
                        get_executor(),
                        [self          = shared_from_this(),
                         sql_statement = sql_statement,
                         timeout,
                         max_rows,
                         messages]() mutable -> net::awaitable< std::unique_ptr< typename ResultSet::impl_type > > {
                            co_return self->connection_->execute_query< typename ResultSet::impl_type >(
                                sql_statement, timeout, max_rows, messages);
                        },
                        net::use_awaitable),
                    get_executor());
            }

            bool connected() const { return connection_->connected(); }

          private:
            // net::strand< net::thread_pool::executor_type > get_executor()
            net::strand< net::system_executor > get_executor()

            {
                assert(exec_.impl_);
                return exec_;
            }

            // net::strand< net::thread_pool::executor_type > exec_;
            net::strand< net::system_executor > exec_;
            std::shared_ptr< sync::connection > connection_;
        };
    }   // namespace detail

    struct connection
    {
        explicit connection()
        : impl_(std::make_shared< detail::connection_wrapper >())
        {
        }

        connection(const connection &other) = delete;
        connection &operator=(const connection &other) = delete;

        net::awaitable< bool > connect(std::string connection_str, std::vector< message > *messages = nullptr)
        {
            co_return co_await impl_->connect(std::move(connection_str), messages);
        }

        net::awaitable< void > disconnect() { co_await impl_->disconnect(); }

        bool connected() const { return impl_->connected(); }

      public:   // execute statements
        net::awaitable< long > execute(std::string &           sql_statement,
                                       std::size_t             timeout  = 0,
                                       std::size_t             max_rows = 0,
                                       std::vector< message > *messages = nullptr)
        {
            co_return co_await impl_->execute(sql_statement, timeout, max_rows, messages);
        }

        template < typename ResultSet >
        net::awaitable< std::unique_ptr< ResultSet > > execute_query(std::string &           sql_statement,
                                                                     std::size_t             timeout  = 0,
                                                                     std::size_t             max_rows = 0,
                                                                     std::vector< message > *messages = nullptr)
        {
            co_return co_await impl_->execute_query< ResultSet >(sql_statement, timeout, max_rows, messages);
        }

      private:
        std::shared_ptr< detail::connection_wrapper > impl_;
    };
}   // namespace aodbc::async
