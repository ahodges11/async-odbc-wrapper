//
// Created by ahodges on 08/10/2020.
//

#pragma once

#include "aodbc.hpp"
#include "row_set/compact_row_set.hpp"

struct result_set_test
{
    void start(std::string connection_str)
    {
        auto env = aodbc::environment();
        env.init();
        auto connection = env.create_connection(std::move(connection_str));
        connection.connect();
        assert(connection.connected());

        auto statement = connection.create_statement();
        auto sql = std::string("select * from testing_more;");
        auto result = statement.execute_query(sql);

        auto rowset = result.create_row_set<aodbc::row_set::compact_row_set>();
        auto rows = result.row_count();
        auto p = 10;

        connection.disconnect();

    }


};