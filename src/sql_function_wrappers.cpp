//
// Created by ahodges on 07/10/2020.
//

#include "sql_function_wrappers.hpp"

#include <string>

namespace aodbc
{
    void handle_diagnostic(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode)
    {
        SQLSMALLINT record_num = 0;
        SQLINTEGER  error_num;
        SQLCHAR     message[1000];
        SQLCHAR     state[SQL_SQLSTATE_SIZE + 1];

        if (retcode == SQL_INVALID_HANDLE)
        {
            fwprintf(stderr, L"Invalid handle!\n");
            return;
        }

        while (SQLGetDiagRec(handle_type,
                             handle,
                             ++record_num,
                             state,
                             &error_num,
                             message,
                             (SQLSMALLINT)(sizeof(message) / sizeof(WCHAR)),
                             (SQLSMALLINT *)nullptr) == SQL_SUCCESS)
        {
            // TODO build type to transport this data
            fwprintf(stderr, L"[%5.5s] %s (%d)\n", state, message, error_num);
        }
    }
    void handle_odbc_call(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode)
    {
        if (retcode == SQL_SUCCESS)
            return;
        handle_diagnostic(handle, handle_type, retcode);
        if (retcode == SQL_SUCCESS_WITH_INFO)
            return;
        if ((retcode == SQL_ERROR) || (retcode == SQL_INVALID_HANDLE))
        {
            throw aodbc_odbc_exception("error in: " + std::to_string(retcode));
        }
    }

    void sql_alloc(SQLHANDLE &handle, SQL_handle_type type)
    {
        handle_odbc_call(handle, type, SQLAllocHandle(type, SQL_NULL_HANDLE, &handle));
    }

    void sql_alloc_parent(SQLHANDLE &handle, SQL_handle_type type, SQLHANDLE *parent)
    {
        handle_odbc_call(*parent, type, SQLAllocHandle(type, *parent, &handle));
    }

    void sql_dealloc(SQLHANDLE &handle, SQL_handle_type type)
    {
        handle_odbc_call(handle, type, SQLFreeHandle(type, handle));
    }

    void sql_alloc_env(SQLHENV &handle_env)
    {
        handle_odbc_call(handle_env, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &handle_env));
    }
    void sql_dealloc_env(SQLHENV &handle_env)
    {
        handle_odbc_call(handle_env, SQL_HANDLE_ENV, SQLFreeHandle(SQL_HANDLE_ENV, &handle_env));
    }
    void sql_set_env_attr(SQLHENV &handle_env, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length)
    {
        handle_odbc_call(handle_env, SQL_HANDLE_ENV, SQLSetEnvAttr(handle_env, attribute, value_ptr, string_length));
    }

    void sql_alloc_dbc(SQLHENV &handle_env, SQLHDBC &handle_dbc)
    {
        handle_odbc_call(handle_env, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_DBC, handle_env, &handle_dbc));
    }
    void sql_dealloc_dbc(SQLHDBC &handle_dbc)
    {
        handle_odbc_call(handle_dbc, SQL_HANDLE_DBC, SQLFreeHandle(SQL_HANDLE_DBC, &handle_dbc));
    }
    void sql_driver_connect(SQLHDBC &handle_dbc, std::string &in_conn_str)
    {
        handle_odbc_call(handle_dbc,
                         SQL_HANDLE_DBC,
                         SQLDriverConnect(handle_dbc,
                                          nullptr,
                                          reinterpret_cast< unsigned char * >(in_conn_str.data()),
                                          SQL_NTS,
                                          nullptr,
                                          0,
                                          nullptr,
                                          SQL_DRIVER_NOPROMPT));
    }
    void sql_driver_disconnect(SQLHDBC &handle_dbc)
    {
        handle_odbc_call(handle_dbc, SQL_HANDLE_DBC, SQLDisconnect(handle_dbc));
    }

    void sql_alloc_stmt(SQLHDBC &handle_dbc, SQLHSTMT &handle_stmt)
    {
        handle_odbc_call(handle_dbc, SQL_HANDLE_DBC, SQLAllocHandle(SQL_HANDLE_STMT, handle_dbc, &handle_stmt));
    }
    void sql_dealloc_stmt(SQLHSTMT &handle_stmt)
    {
        handle_odbc_call(handle_stmt, SQL_HANDLE_STMT, SQLFreeHandle(SQL_HANDLE_STMT, handle_stmt));
    }
    void sql_exec_direct(SQLHSTMT &handle_stmt, std::string &statement)
    {
        handle_odbc_call(
            handle_stmt,
            SQL_HANDLE_STMT,
            SQLExecDirect(handle_stmt, reinterpret_cast< unsigned char * >(statement.data()), statement.size()));
    }
    void sql_row_count(SQLHSTMT &handle_stmt, SQLLEN *row_count)
    {
        handle_odbc_call(handle_stmt, SQL_HANDLE_STMT, SQLRowCount(handle_stmt, row_count));
    }
    void sql_num_result_cols(SQLHSTMT &handle_stmt, SQLSMALLINT *num_result_cols)
    {
        handle_odbc_call(handle_stmt, SQL_HANDLE_STMT, SQLNumResultCols(handle_stmt, num_result_cols));
    }
    void sql_set_max_rows(SQLHSTMT &handle_stmt, SQLULEN max_rows)
    {
        handle_odbc_call(handle_stmt,
                         SQL_HANDLE_STMT,
                         SQLSetStmtAttr(handle_stmt,
                                        SQL_ATTR_MAX_ROWS,
                                        reinterpret_cast< SQLPOINTER >(static_cast< uintptr_t >(max_rows)),
                                        SQL_IS_UINTEGER));
    }
    SQLULEN sql_get_max_rows(SQLHSTMT &handle_stmt)
    {
        SQLULEN max_rows;
        handle_odbc_call(
            handle_stmt,
            SQL_HANDLE_STMT,
            SQLGetStmtAttr(handle_stmt, SQL_ATTR_MAX_ROWS, reinterpret_cast< SQLPOINTER >(&max_rows), 0, nullptr));
        return max_rows;
    }
    void sql_set_query_timeout(SQLHSTMT &handle_stmt, SQLULEN seconds)
    {
        handle_odbc_call(handle_stmt,
                         SQL_HANDLE_STMT,
                         SQLSetStmtAttr(handle_stmt,
                                        SQL_ATTR_QUERY_TIMEOUT,
                                        reinterpret_cast< SQLPOINTER >(static_cast< uintptr_t >(seconds)),
                                        SQL_IS_UINTEGER));
    }
    SQLULEN sql_get_query_timeout(SQLHSTMT &handle_stmt)
    {
        SQLULEN seconds;
        handle_odbc_call(
            handle_stmt,
            SQL_HANDLE_STMT,
            SQLGetStmtAttr(handle_stmt, SQL_ATTR_QUERY_TIMEOUT, reinterpret_cast< SQLPOINTER >(&seconds), 0, nullptr));
        return seconds;
    }
    void sql_free_stmt(SQLHSTMT &handle_stmt, SQLUSMALLINT option)
    {
        handle_odbc_call(handle_stmt, SQL_HANDLE_STMT, SQLFreeStmt(handle_stmt, option));
    }

    void sql_set_row_size(SQLHSTMT &handle_stmt, SQLLEN row_size)
    {
        handle_odbc_call(handle_stmt,
                         SQL_HANDLE_STMT,
                         SQLSetStmtAttr(handle_stmt, SQL_ATTR_ROW_ARRAY_SIZE, (SQLPOINTER)row_size, SQL_IS_UINTEGER));
    }

    SQLLEN sql_get_row_size(SQLHSTMT &handle_stmt)
    {
        SQLLEN result;
        handle_odbc_call(
            handle_stmt,
            SQL_HANDLE_STMT,
            SQLGetStmtAttr(handle_stmt, SQL_ATTR_ROW_ARRAY_SIZE, reinterpret_cast< SQLPOINTER >(&result), 0, nullptr));
        return result;
    }

}   // namespace aodbc