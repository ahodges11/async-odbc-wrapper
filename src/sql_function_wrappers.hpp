//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "exception.hpp"

#include <sql.h>
#include <sqlext.h>
#include <string>

namespace aodbc
{
    typedef decltype(SQL_HANDLE_ENV) SQL_handle_type;

    struct message
    {
        SQLINTEGER  error_number;
        std::string text;
        SQLCHAR     state[SQL_SQLSTATE_SIZE + 1];
    };

    void
    handle_diagnostic(SQLHANDLE &handle, SQLSMALLINT handle_type, std::vector< message > *messages);
    void handle_diagnostic(SQLHANDLE &handle, SQLSMALLINT handle_type);
    void
    handle_odbc_call(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode, std::vector< message > *messages);
    void handle_odbc_call(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode);

    // --------------- ALLOC -----------------
    void sql_alloc(SQLHANDLE &handle, SQL_handle_type type);
    void sql_alloc_parent(SQLHANDLE &handle, SQL_handle_type type, SQLHANDLE *parent);
    void sql_dealloc(SQLHANDLE &handle, SQL_handle_type type);

    // --------------- ENVIRONMENT RELATED CALLS ---------------------

    void sql_alloc_env(SQLHENV &handle_env);
    void sql_dealloc_env(SQLHENV &handle_env);
    void sql_set_env_attr(SQLHENV &handle_env, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length);

    // --------------- DATABASE CONNECTION RELATED CALLS ---------------------
    void sql_alloc_dbc(SQLHENV &handle_env, SQLHDBC *handle_dbc);
    void sql_dealloc_dbc(SQLHDBC &handle_dbc);
    void sql_driver_connect(SQLHDBC &handle_dbc, std::string &in_conn_str, std::vector< message > *messages);
    void sql_driver_connect(SQLHDBC &handle_dbc, std::string &in_conn_str);
    void sql_driver_disconnect(SQLHDBC &handle_dbc);

    // --------------- DATABASE STATEMENT RELATED CALLS ---------------------
    void    sql_alloc_stmt(SQLHDBC &handle_dbc, SQLHSTMT *handle_stmt);
    void    sql_dealloc_stmt(SQLHSTMT &handle_stmt);
    void    sql_exec_direct(SQLHSTMT &handle_stmt, std::string &statement);
    void    sql_row_count(SQLHSTMT &handle_stmt, SQLLEN *row_count);
    void    sql_num_result_cols(SQLHSTMT &handle_stmt, SQLSMALLINT *num_result_cols);
    void    sql_set_max_rows(SQLHSTMT &handle_stmt, SQLULEN max_rows);
    SQLULEN sql_get_max_rows(SQLHSTMT &handle_stmt);
    void    sql_set_query_timeout(SQLHSTMT &handle_stmt, SQLULEN seconds);
    SQLULEN sql_get_query_timeout(SQLHSTMT &handle_stmt);
    void    sql_free_stmt(SQLHSTMT &handle_stmt, SQLUSMALLINT option);
    void    sql_set_row_size(SQLHSTMT &handle_stmt, SQLLEN row_size);
    SQLLEN  sql_get_row_size(SQLHSTMT &handle_stmt);

    void sql_set_array_size(SQLHSTMT &handle_stmt, SQLULEN array_size);
    void sql_fetch_scroll(SQLHSTMT &handle_stmt, SQLSMALLINT orientation, SQLROWOFFSET offset);
    void sql_set_row_status_ptr(SQLHSTMT &handle_stmt, SQLUSMALLINT *row_status);
    void sql_set_rows_fetched_ptr(SQLHSTMT &handle_stmt, SQLROWSETSIZE *row_count);

    void sql_bind_col(SQLHSTMT &   handle_stmt,
                      SQLUSMALLINT col_number,
                      SQLSMALLINT  col_type,
                      SQLPOINTER   data_ptr,
                      SQLLEN       buf_len,
                      SQLLEN *     indicator_ptr);

}   // namespace aodbc
