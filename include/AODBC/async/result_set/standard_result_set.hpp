//
// Created by ahodges on 02/11/2020.
//

#pragma once

#include <AODBC/config.hpp>
#include <AODBC/sync/result_set/standard_result_set.hpp>

namespace aodbc::async::result_set
{
    namespace detail
    {
        struct sync_wrapper
        {
            typedef sync::result_set::standard_result_set impl_type;

            sync_wrapper(std::unique_ptr< impl_type > impl, net::any_io_executor exec)
            : exec_(std::move(exec))
            , impl_(std::move(impl))
            {
            }

            const sync::result_set::result_set_metadata &get_metadata() { return impl_->get_metadata(); }

            net::awaitable< bool > next()
            {
                co_return co_await net::co_spawn(
                    get_executor(),
                    [&]() -> net::awaitable< bool > { co_return impl_->next(); },
                    net::use_awaitable);
            }

            types::aodbc_bit       get_bit(std::size_t column_index) { return impl_->get_bit(column_index); };
            types::aodbc_bigint    get_bigint(std::size_t column_index) { return impl_->get_bigint(column_index); };
            types::aodbc_int       get_int(std::size_t column_index) { return impl_->get_int(column_index); };
            types::aodbc_short     get_short(std::size_t column_index) { return impl_->get_short(column_index); };
            types::aodbc_tinyint   get_tinyint(std::size_t column_index) { return impl_->get_tinyint(column_index); };
            types::aodbc_float     get_float(std::size_t column_index) { return impl_->get_float(column_index); };
            types::aodbc_double    get_double(std::size_t column_index) { return impl_->get_double(column_index); };
            types::aodbc_varchar   get_varchar(std::size_t column_index) { return impl_->get_varchar(column_index); };
            types::aodbc_nvarchar  get_nvarchar(std::size_t column_index) { return impl_->get_nvarchar(column_index); };
            types::aodbc_binary    get_binary(std::size_t column_index) { return impl_->get_binary(column_index); };
            types::aodbc_date      get_date(std::size_t column_index) { return impl_->get_date(column_index); };
            types::aodbc_time      get_time(std::size_t column_index) { return impl_->get_time(column_index); };
            types::aodbc_timestamp get_timestamp(std::size_t column_index)
            {
                return impl_->get_timestamp(column_index);
            };
            types::aodbc_decimal get_decimal(std::size_t column_index) { return impl_->get_decimal(column_index); };

            template < typename DesiredType >
            bool column_is_type(std::size_t column_index)
            {
                return impl_->column_is_type< DesiredType >(column_index);
            }

            std::size_t key_to_column_index(const std::string &key) { return impl_->key_to_column_index(key); }

          private:
            net::any_io_executor get_executor() {return exec_;}

            net::any_io_executor         exec_;
            std::unique_ptr< impl_type > impl_;
        };
    }   // namespace detail

    struct standard_result_set
    {
        using impl_type = detail::sync_wrapper::impl_type;

        standard_result_set(std::unique_ptr< detail::sync_wrapper::impl_type > underlying,
                            net::any_io_executor               exec)
        : impl_(std::move(underlying), std::move(exec))
        {
        }

        // metadata is threadsafe after `init()` is called on the underlying sync resultset, so a reference is safe
        // here.
        const sync::result_set::result_set_metadata &get_metadata() { return impl_.get_metadata(); }

        net::awaitable< bool > next() { co_return co_await impl_.next(); }

        types::aodbc_bit       get_bit(std::size_t column_index) { return impl_.get_bit(column_index); };
        types::aodbc_bigint    get_bigint(std::size_t column_index) { return impl_.get_bigint(column_index); };
        types::aodbc_int       get_int(std::size_t column_index) { return impl_.get_int(column_index); };
        types::aodbc_short     get_short(std::size_t column_index) { return impl_.get_short(column_index); };
        types::aodbc_tinyint   get_tinyint(std::size_t column_index) { return impl_.get_tinyint(column_index); };
        types::aodbc_float     get_float(std::size_t column_index) { return impl_.get_float(column_index); };
        types::aodbc_double    get_double(std::size_t column_index) { return impl_.get_double(column_index); };
        types::aodbc_varchar   get_varchar(std::size_t column_index) { return impl_.get_varchar(column_index); };
        types::aodbc_nvarchar  get_nvarchar(std::size_t column_index) { return impl_.get_nvarchar(column_index); };
        types::aodbc_binary    get_binary(std::size_t column_index) { return impl_.get_binary(column_index); };
        types::aodbc_date      get_date(std::size_t column_index) { return impl_.get_date(column_index); };
        types::aodbc_time      get_time(std::size_t column_index) { return impl_.get_time(column_index); };
        types::aodbc_timestamp get_timestamp(std::size_t column_index) { return impl_.get_timestamp(column_index); };
        types::aodbc_decimal   get_decimal(std::size_t column_index) { return impl_.get_decimal(column_index); };

        template < typename DesiredType >
        bool column_is_type(std::size_t column_index)
        {
            return impl_.column_is_type< DesiredType >(column_index);
        }

        std::size_t key_to_column_index(const std::string &key) { return impl_.key_to_column_index(key); }

      private:
        detail::sync_wrapper impl_;
    };
}   // namespace aodbc::async::result_set
