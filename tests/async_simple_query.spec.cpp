//
// Created by ahodges on 03/11/2020.
//
#include "arguments.hpp"
#include "exception_handler.hpp"
#include "net.hpp"

#include <AODBC/aodbc.hpp>
#include <AODBC/async/result_set/standard_result_set.hpp>

#include <catch2/catch.hpp>
#include <cstdlib>

net::awaitable< net::any_io_executor > get_this_exec()
{
    co_return co_await net::this_coro::executor;
}

/*
struct component_test
{
    net::awaitable< void > start()
    {
        auto strand = net::make_strand(aodbc::async::get_thread_pool().executor());
        co_await net::co_spawn(
            strand,
            []() mutable -> net::awaitable< void > {
                auto exec = co_await get_this_exec();

                std::vector< aodbc::message > messages;
                aodbc::handles::dbc_handle    dbc(aodbc::get_default_env().get_env());
                aodbc::sql_driver_connect(dbc.get_handle(), connection_str, &messages);
                if (not messages.empty())
                {
                    for (auto &msg : messages)   // print
                    {
                        aodbc::log_debug(msg.to_string().data());
                    }

                    messages.clear();
                }

                auto timer = net::high_resolution_timer(exec);
                timer.expires_after(std::chrono::milliseconds(5));
                co_await timer.async_wait(net::use_awaitable);
                // aodbc::sql_driver_disconnect(dbc.get_handle());
            },
            net::use_awaitable);
    }
};

struct impl_connect_test
{
    net::awaitable< void > start(std::string &str)
    {
        auto origin_exec = co_await net::this_coro::executor;
        auto exec        = aodbc::async::get_thread_pool().get_executor();
        auto connection  = aodbc::sync::connection();

        // connect

        auto timer = net::high_resolution_timer(co_await net::this_coro::executor);
        timer.expires_at(std::chrono::high_resolution_clock::time_point::max());

        net::post(exec, [&]() {
            connection.connect(str);
            timer.cancel();
        });

        auto ec = boost::system::error_code();
        co_await timer.async_wait(net::redirect_error(net::use_awaitable, ec));

    }
};

struct co_await_test
{
    net::awaitable<void > start(std::string & str)
        {
            auto exec        = aodbc::async::get_thread_pool().get_executor();
            auto conn = aodbc::sync::connection();


            co_await net::co_spawn(exec, [&]() -> net::awaitable<void>
                {
                    conn.connect(str);
                    co_return;
                }, net::use_awaitable);

        }
};

struct connect_test
{
    net::awaitable< void > start(std::string &conn_str)
    {
        auto exec       = co_await get_this_exec();
        auto connection = aodbc::async::connection();
        try
        {
            co_await connection.connect(conn_str);
            assert(exec == co_await get_this_exec());
        }
        catch (...)
        {
            std::cout << "error!" << std::endl;
        }

        // co_await connection.disconnect();
    }
};

 */
struct single_query
{
    net::awaitable< void > start(std::string &conn_str)
    {
        auto messages   = std::vector< aodbc::message >();
        auto connection = aodbc::async::connection();
        co_await connection.connect(conn_str, &messages);
        {
            // Execute sql
            auto sql     = std::string("select * from testing_more with (nolock);");
            auto res_set = co_await connection.execute_query< aodbc::async::result_set::standard_result_set >(sql);
            auto results = co_await res_set->next();   // Increment the cursor
            CHECK(results);

            // Check the column types
            CHECK(res_set->column_is_type< aodbc::types::aodbc_bit >(1));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_bigint >(2));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_int >(3));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_short >(4));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_tinyint >(5));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_varchar >(6));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_varchar >(7));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_float >(8));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_double >(9));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_date >(10));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_time >(11));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_timestamp >(12));
            CHECK(res_set->column_is_type< aodbc::types::aodbc_decimal >(13));

            // Get the results
            auto bool_column          = res_set->get_bit(1);
            auto bigint_column        = res_set->get_bigint(2);
            auto int_column           = res_set->get_int(3);
            auto smallint_column      = res_set->get_short(4);
            auto tinyint_column       = res_set->get_tinyint(5);
            auto small_varchar_column = res_set->get_varchar(6);
            auto large_varchar_column = res_set->get_varchar(7);
            auto float_column         = res_set->get_float(8);
            auto double_column        = res_set->get_double(9);
            auto date_column          = res_set->get_date(10);
            auto time_column          = res_set->get_time(11);
            auto datetime_column      = res_set->get_timestamp(12);
            auto decimal_column       = res_set->get_decimal(13);
            boost::ignore_unused(decimal_column);

            CHECK(*bool_column);
            CHECK(*bigint_column == 2);
            CHECK(*int_column == 3);
            CHECK(*smallint_column == 4);
            CHECK(*tinyint_column == 5);
            CHECK(*small_varchar_column == "small_vchar");
            CHECK(*large_varchar_column == "this is a much larger vchar, SPACE SPACE SPACE SPACE SPACE SPACE "
                                           "SPACE SPACE SPACE SPACE SPACE SPACE SPACE SPACE SPACE ");
            CHECK(trunc(*float_column * 1000000.) == trunc(3.141592 * 1000000.));
            CHECK(trunc(*double_column * 1000000000000000.) == trunc(3.141592653589793 * 1000000000000000.));
            auto expected_date = aodbc::types::date(1999, 11, 25);
            CHECK(*date_column == expected_date);
            auto expected_time = aodbc::types::time(12, 0, 1);
            CHECK(*time_column == expected_time);
            auto expected_timestamp = aodbc::types::timestamp(expected_date.year,
                                                              expected_date.month,
                                                              expected_date.day,
                                                              expected_time.hour,
                                                              expected_time.minute,
                                                              expected_time.second,
                                                              0);
            CHECK(*datetime_column == expected_timestamp);
            // auto expected_decimal = aodbc::types::decimal();
            // TODO
            // expected_decimal.get_impl()
            // CHECK( == );
        }

        co_await connection.disconnect();   // this deallocs the statement handle in res_set automatically.
    }
};

struct multiple_queries
{
    net::awaitable< void > start(std::string &con_str, std::size_t queries)
    {
        std::vector< aodbc::message > messages;

        auto check_messages = [&messages](auto context) {
            if (not messages.empty())
            {
                std::cout << "message(s) received in sql_func: " << context << "\nMessages: \n";
                for (auto &msg : messages)
                {
                    std::cout << msg.to_string() << "\n";
                }
                std::cout << std::endl;
                messages.clear();
            }
        };

        auto connection = aodbc::async::connection();
        co_await connection.connect(con_str);

        auto sql = std::string("select * from testing_bulk with (nolock);");
        // execute this statement multiple times, checking the data each time
        for (std::size_t x = 0; x < queries; x++)
        {
            auto res_set = co_await connection.execute_query< aodbc::async::result_set::standard_result_set >(
                sql, 0, 0, &messages);
            check_messages("connection.execute_query()");

            auto count = 0;
            while (co_await res_set->next())
            {
                count++;

                // CHECK TYPES
                CHECK(res_set->column_is_type< aodbc::types::aodbc_bit >(1));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_bigint >(2));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_int >(3));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_short >(4));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_tinyint >(5));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_varchar >(6));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_varchar >(7));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_float >(8));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_double >(9));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_date >(10));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_time >(11));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_timestamp >(12));
                CHECK(res_set->column_is_type< aodbc::types::aodbc_decimal >(13));

                // GET DATA
                auto bool_column          = res_set->get_bit(1);
                auto bigint_column        = res_set->get_bigint(2);
                auto int_column           = res_set->get_int(3);
                auto smallint_column      = res_set->get_short(4);
                auto tinyint_column       = res_set->get_tinyint(5);
                auto small_varchar_column = res_set->get_varchar(6);
                auto large_varchar_column = res_set->get_varchar(7);
                auto float_column         = res_set->get_float(8);
                auto double_column        = res_set->get_double(9);
                auto date_column          = res_set->get_date(10);
                auto time_column          = res_set->get_time(11);
                auto datetime_column      = res_set->get_timestamp(12);
                auto decimal_column       = res_set->get_decimal(13);
                boost::ignore_unused(decimal_column);

                // CHECK DATA
                CHECK(*bool_column);
                CHECK(*bigint_column == count + 1);
                CHECK(*int_column == count + 2);
                CHECK(*smallint_column == count + 3);
                CHECK(*tinyint_column == count + 4);
                auto str_count = std::to_string(count);
                CHECK(*small_varchar_column == str_count);
                CHECK(*large_varchar_column == "longer_string_one_two_three_words_words" + str_count);

                CHECK(*float_column == Approx(3.141592 + count).margin(0.000001));
                CHECK(*double_column == Approx(3.141592653589793 + count).margin(0.0000000001));
                auto expected_date = aodbc::types::date(1999, 11, 25);
                CHECK(*date_column == expected_date);
                auto expected_time = aodbc::types::time(12, 0, 1);
                CHECK(*time_column == expected_time);
                auto expected_timestamp = aodbc::types::timestamp(expected_date.year,
                                                                  expected_date.month,
                                                                  expected_date.day,
                                                                  expected_time.hour,
                                                                  expected_time.minute,
                                                                  expected_time.second,
                                                                  0);
                CHECK(*datetime_column == expected_timestamp);
                // auto expected_decimal = aodbc::types::decimal();
                // TODO
                // expected_decimal.get_impl()
                // CHECK( == );
            }
            CHECK(count == 251);
        }
        co_await connection.disconnect();
        CHECK_FALSE(connection.connected());
    }
};

TEST_CASE("async simple query")
{
    aodbc::async::get_thread_pool();
    auto ioc = net::io_context(1);

    SECTION("1 connection - 1 query")
    {
        aodbc::net::co_spawn(
            net::prefer(ioc.get_executor(), net::execution::outstanding_work.tracked),
            [&]() -> aodbc::net::awaitable< void > {
                auto test = single_query();
                co_await test.start(connection_str);
            },
            [](auto ptr) { exception_handler(ptr, "async test"); });

        auto start = std::chrono::high_resolution_clock::now();
        ioc.run();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "async: 1 connections - 1 query took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("10 connections - 1 query")
    {
        for (int x = 0; x < 10; x++)
        {
            aodbc::net::co_spawn(
                net::prefer(ioc.get_executor(), net::execution::outstanding_work.tracked),
                [&]() -> aodbc::net::awaitable< void > {
                    auto test = single_query();
                    co_await test.start(connection_str);
                },
                [](auto ptr) { exception_handler(ptr, "async test"); });
        }
        auto start = std::chrono::high_resolution_clock::now();
        ioc.run();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "async: 10 connections - 1 query took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("1000 connections - 1 query")
    {
        for (int x = 0; x < 1000; x++)
        {
            aodbc::net::co_spawn(
                net::prefer(ioc.get_executor(), net::execution::outstanding_work.tracked),
                [&]() -> aodbc::net::awaitable< void > {
                    auto test = single_query();
                    co_await test.start(connection_str);
                },
                [](auto ptr) { exception_handler(ptr, "async: 10000 connections - 1 query"); });
        }
        auto start = std::chrono::high_resolution_clock::now();

        try
        {
            ioc.run();
        }
        catch (std::exception &ec)
        {
            aodbc::log_fatal(fmt::format("exception caught in ioc.run() - {}", ec.what()).data());
            aodbc::async::get_thread_pool().stop();
            aodbc::async::get_thread_pool().join();
            std::exit(100);
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "async: 1000 connections - 1 query took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("1 connection - 2 queries")
    {
        aodbc::net::co_spawn(
            net::prefer(ioc.get_executor(), net::execution::outstanding_work.tracked),
            [&]() -> aodbc::net::awaitable< void > {
                auto test = multiple_queries();
                co_await test.start(connection_str, 2);
            },
            [](auto ptr) { exception_handler(ptr, "1 connection - multiple queries"); });

        auto start = std::chrono::high_resolution_clock::now();
        ioc.run();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "async: 1 connections - 2 queries took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("1 connection - 100 queries")
    {
        aodbc::net::co_spawn(
            net::prefer(ioc.get_executor(), net::execution::outstanding_work.tracked),
            [&]() -> aodbc::net::awaitable< void > {
                auto test = multiple_queries();
                co_await test.start(connection_str, 100);
            },
            [](auto ptr) { exception_handler(ptr, "1 connection - multiple queries"); });

        auto start = std::chrono::high_resolution_clock::now();
        ioc.run();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "async: 1 connections - 100 queries took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }
}
