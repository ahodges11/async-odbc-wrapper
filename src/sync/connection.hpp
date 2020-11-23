//
// Created by ahodges on 26/10/2020.
//

#pragma once

#include "environment.hpp"
#include "statement.hpp"

#include <boost/core/ignore_unused.hpp>

namespace aodbc::sync
{
    struct connection
    {
        connection(environment &env = get_default_env())
        : dbc_(env.get_env())
        , connected_(false)
        {
            // make sure the handle got allocated correctly
            assert(dbc_.get_handle() != SQL_NULL_HDBC);
        }

        ~connection()
        {
            if (connected())
            {
                disconnect();
                log_warning("connection destroyed while it was still connected.");
            }
        }

      public:   // startup/shutdown
        void connect(std::string connection_str)
        {
            assert(not connected());
            // connect to the driver
            sql_driver_connect(dbc_.get_handle(), connection_str);
            connected_ = true;
        }
        void connect(std::string &connection_str, std::vector< message > *messages=nullptr)
        {
            assert(not connected());
            sql_driver_connect(dbc_.get_handle(), connection_str, messages);
            connected_ = true;
        }
        void disconnect()
        {
            assert(connected());
            sql_driver_disconnect(dbc_.get_handle());
            dbc_.get_self()->mark_children_dead();
            connected_ = false;
        }

      public:   // query state
        bool connected() const { return connected_; }

      public:   // execute sql statement
        long execute(std::string &sql_statement, std::size_t timeout = 0, std::size_t max_rows = 0)
        {
            auto statement = create_statement();
            if (timeout != statement::DEFAULT_TIMEOUT)
                statement.set_query_timeout(timeout);
            if (max_rows != statement::DEFAULT_MAX_ROWS)
                statement.set_max_rows(max_rows);

            return statement.execute(sql_statement);
        }

        template < typename ResultSet >
        std::unique_ptr< ResultSet > execute_query(std::string &           sql_statement,
                                                   std::size_t             timeout  = 0,
                                                   std::size_t             max_rows = 0,
                                                   std::vector< message > *messages = nullptr)
        {
            auto statement = create_statement();
            if (timeout != statement::DEFAULT_TIMEOUT)
                statement.set_query_timeout(timeout);
            if (max_rows != statement::DEFAULT_MAX_ROWS)
                statement.set_max_rows(max_rows);

            return statement.execute_query< ResultSet >(sql_statement, messages);
        }

        statement create_statement() { return statement(dbc_); }

      private:                      // data members
        handles::dbc_handle dbc_;   // the underlying odbc connection handle
        bool                connected_;
    };

}   // namespace aodbc::sync