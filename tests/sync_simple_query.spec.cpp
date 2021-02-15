//
// Created by ahodges on 03/11/2020.
//

#include "aodbc.hpp"
#include "arguments.hpp"
#include "sync/result_set/standard_result_set.hpp"

#include <catch2/catch.hpp>

struct simple_interface_test
{
    void start(std::string conn_str)
    {
        auto connection = aodbc::sync::connection();
        connection.connect(conn_str, nullptr);
        assert(connection.connected());

        {
            // Execute sql
            auto sql     = std::string("select * from testing_more;");
            auto res_set = connection.execute_query< aodbc::sync::result_set::standard_result_set >(sql);
            auto results = res_set->next();   // Increment the cursor

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
            // throw_if_false( == );
        }

        connection.disconnect();   // this deallocs the statement handle in res_set automatically.
    }
};

struct multiple_queries_text
{
    void start(std::string &conn_str, std::size_t queries)
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

        // connect to database
        auto connection = aodbc::sync::connection();
        connection.connect(conn_str, nullptr);
        assert(connection.connected());

        for (std::size_t x = 0; x < queries; x++)
        {
            // execute query
            auto sql = std::string("select * from testing_bulk;");
            auto res_set =
                connection.execute_query< aodbc::sync::result_set::standard_result_set >(sql, 0, 0, &messages);
            check_messages("connection.execute_query()");

            auto count = 0;
            while (res_set->next(&messages))
            {
                check_messages("result_set.next()");
                count++;

                // check types
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

                // fetch results
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

                // check results
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

        connection.disconnect();
        CHECK_FALSE(connection.connected());
    }
};

TEST_CASE("sync simple query")
{
    auto start = std::chrono::high_resolution_clock::now();
    SECTION("1 connection - 1 query")
    {
        auto test = simple_interface_test();
        test.start(connection_str);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync: 1 connections - 1 query took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("100 connection - 1 query")
    {
        for (auto x = 0; x < 100; x++)
        {
            auto test = simple_interface_test();
            test.start(connection_str);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync: 100 connections - 1 query took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("1 connection - 2 queries")
    {
        auto test = multiple_queries_text();
        test.start(connection_str, 2);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync: 1 connections - 2 queries took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }

    SECTION("1 connection - 100 queries")
    {
        auto test = multiple_queries_text();
        test.start(connection_str, 100);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync: 1 connections - 100 queries took: " << (end - start).count() << "(ns) "
                  << std::chrono::duration_cast< std::chrono::milliseconds >((end - start)).count() << "(ms)"
                  << std::endl;
    }
}
