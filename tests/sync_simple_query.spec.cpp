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
        connection.connect(conn_str);
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

TEST_CASE("sync simple query")
{
    SECTION("1 connection - 1 query")
    {
        auto test = simple_interface_test();
        test.start(connection_str);
    }
}