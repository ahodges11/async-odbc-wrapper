//
// Created by ahodges on 08/10/2020.
//

#pragma once

#include "aodbc.hpp"

struct basic_test
{
    void start(std::string connection_str)
    {
        auto env = aodbc::environment();
        env.init();
        auto connection = env.create_connection(std::move(connection_str));
        connection.connect();
        assert(connection.connected());

        {
            auto stmt    = connection.create_statement();
            auto timeout = stmt.get_query_timeout();
            assert(timeout == 0);
            stmt.set_query_timeout(5);
            timeout = stmt.get_query_timeout();
            assert(timeout == 5);


            // Execute sql
            auto sql     = std::string("select * from testing_more;");
            auto res_set = stmt.execute_query(sql);
            res_set.next();   // Increment the cursor

            // Get the results
            auto bool_column          = res_set.get< aodbc::types::aodbc_bit >(1);
            auto bigint_column        = res_set.get< aodbc::types::aodbc_bigint >(2);
            auto int_column           = res_set.get< aodbc::types::aodbc_int >(3);
            auto smallint_column      = res_set.get< aodbc::types::aodbc_short >(4);
            auto tinyint_column       = res_set.get< aodbc::types::aodbc_tinyint >(5);
            auto small_varchar_column = res_set.get< aodbc::types::aodbc_varchar >(6);
            auto large_varchar_column = res_set.get< aodbc::types::aodbc_varchar >(7);
            auto float_column         = res_set.get< aodbc::types::aodbc_float >(8);
            auto double_column        = res_set.get< aodbc::types::aodbc_double >(9);
            auto date_column          = res_set.get< aodbc::types::aodbc_date >(10);
            auto time_column          = res_set.get< aodbc::types::aodbc_time >(11);
            auto datetime_column      = res_set.get< aodbc::types::aodbc_timestamp >(12);
            auto decimal_column       = res_set.get< aodbc::types::aodbc_decimal >(13);

            // Check the results
            auto &bool_column_val          = std::get< aodbc::types::aodbc_bit >(bool_column);
            auto &bigint_column_val        = std::get< aodbc::types::aodbc_bigint >(bigint_column);
            auto &int_column_val           = std::get< aodbc::types::aodbc_int >(int_column);
            auto &smallint_column_val      = std::get< aodbc::types::aodbc_short >(smallint_column);
            auto &tinyint_column_val       = std::get< aodbc::types::aodbc_tinyint >(tinyint_column);
            auto &small_varchar_column_val = std::get< aodbc::types::aodbc_varchar >(small_varchar_column);
            auto &large_varchar_column_val = std::get< aodbc::types::aodbc_varchar >(large_varchar_column);
            auto &float_column_val    = std::get< aodbc::types::aodbc_float >(float_column);
            auto &double_column_val   = std::get< aodbc::types::aodbc_double >(double_column);
            auto &date_column_val     = std::get< aodbc::types::aodbc_date >(date_column);
            auto &time_column_val     = std::get< aodbc::types::aodbc_time >(time_column);
            auto &datetime_column_val = std::get< aodbc::types::aodbc_timestamp >(datetime_column);
            auto &decimal_column_val  = std::get< aodbc::types::aodbc_decimal >(decimal_column);

            auto p = 10;

        }

        connection.disconnect();
    }

};