//
// Created by ahodges on 10/11/2020.
//
#include "arguments.hpp"
#include "sync.hpp"

#include <catch2/catch.hpp>

void check_values(const aodbc::sync::result_set::result_view & view, std::size_t total_rows)
{
    for (std::size_t x = 1; x <= total_rows; x++)
    {
        auto vbit = view.get_bit(x, 1);
        CHECK(!vbit.null());
        CHECK(*vbit);

        auto vbigint = view.get_bigint(x, 2);
        CHECK(!vbigint.null());
        CHECK(*vbigint == static_cast< long >(x + 1));

        auto vint = view.get_int(x, 3);
        CHECK(!vint.null());
        CHECK(*vint == static_cast< int >(x + 2));

        auto vsmall = view.get_short(x, 4);
        CHECK(!vsmall.null());
        CHECK(*vsmall == static_cast< short >(x + 3));

        auto vtinyiny = view.get_tinyint(x, 5);
        CHECK(!vtinyiny.null());
        CHECK(*vtinyiny == static_cast< std::uint8_t >(x + 4));

        auto vshortstr = view.get_string_view(x, 6);
        CHECK(!vshortstr.null());
        CHECK(*vshortstr == std::to_string(x));

        auto vlongstr = view.get_string_view(x, 7);
        CHECK(!vlongstr.null());
        CHECK(*vlongstr == "longer_string_one_two_three_words_words" + std::to_string(x));

        auto vfloat = view.get_float(x, 8);
        CHECK(!vfloat.null());
        CHECK(*vfloat == Approx(3.141592 + x).margin(0.000001));

        auto vdouble = view.get_double(x, 9);
        CHECK(!vdouble.null());
        CHECK(*vdouble == Approx(3.141592653589793 + x).margin(0.0000000001));

        auto vdate = view.get_date(x, 10);
        CHECK(!vdate.null());
        CHECK(*vdate == aodbc::types::date(1999, 11, 25));

        auto vtime = view.get_time(x, 11);
        CHECK(!vtime.null());
        CHECK(*vtime == aodbc::types::time(12, 0, 1));

        auto vtimestamp = view.get_timestamp(x, 12);
        CHECK(!vtimestamp.null());
        CHECK(*vtimestamp == aodbc::types::timestamp(1999, 11, 25, 12, 0, 1, 0));

        auto vdeciaml = view.get_decimal(x, 13);
        CHECK(!vdeciaml.null());
        // CHECK(*vdeciaml ==); TODO check this
    }
}


/*
TEST_CASE("sync bulk query")
{
    auto start = std::chrono::high_resolution_clock::now();



    SECTION("fetch one")
    {
        auto connection = aodbc::sync::connection();
        connection.connect(connection_str, nullptr);
        CHECK(connection.connected());

        constexpr std::size_t total_rows = 251;
        std::string           sql        = "select * from testing_bulk;";
        auto                  res_set = connection.execute_query< aodbc::sync::result_set::bulk_result_set >(sql);

        bool more_results = true;
        while (more_results)
        {
            CHECK(more_results);
            more_results = res_set->fetch_one();
        }

        // get result_view
        auto view = res_set->get_result_view();
        CHECK(view.row_count() == total_rows);

        // Check all rows
        check_values(view,total_rows);


        connection.disconnect();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync-bulk-query: 1 connections - 1 query - fetch_one() took: " << (end - start).count() << std::endl;
    }

    SECTION("fetch many")
    {
        auto connection = aodbc::sync::connection();
        connection.connect(connection_str, nullptr);
        CHECK(connection.connected());

        constexpr std::size_t total_rows = 251;
        std::string           sql        = "select * from testing_bulk;";
        auto                  res_set    = connection.execute_query< aodbc::sync::result_set::bulk_result_set >(sql);

        bool more_rows = true;
        while (more_rows)
        {
            more_rows = res_set->fetch_many(50);
        }

        auto view = res_set->get_result_view();
        CHECK(view.row_count() == total_rows);

        check_values(view,total_rows);

        connection.disconnect();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync-bulk-query: 1 connections - 1 query - fetch_many(50) took: " << (end - start).count() << std::endl;
    }

    SECTION("fetch all")
    {
        auto connection = aodbc::sync::connection();
        connection.connect(connection_str, nullptr);
        CHECK(connection.connected());

        constexpr std::size_t total_rows = 251;
        std::string           sql        = "select * from testing_bulk;";
        auto                  res_set    = connection.execute_query< aodbc::sync::result_set::bulk_result_set >(sql);

        res_set->fetch_all();

        auto view = res_set->get_result_view();
        check_values(view,total_rows);

        connection.disconnect();
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "sync-bulk-query: 1 connections - 1 query - fetch_all() took: " << (end - start).count() << std::endl;
    }
}
 */