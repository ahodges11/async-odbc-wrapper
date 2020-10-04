//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include "exception.hpp"

#include <sql.h>
#include <sqlext.h>

namespace aodbc
{
    void handle_diagnostic(SQLHANDLE handle, SQLSMALLINT handle_type, RETCODE retcode)
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

    void handle_odbc_call(SQLHANDLE handle, SQLSMALLINT handle_type, RETCODE retcode)
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

    // --------------- ENVIRONMENT RELATED CALLS ---------------------

    void sql_alloc_env(SQLHENV *handle_env)
    {
        handle_odbc_call(*handle_env, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, handle_env));
    }

    void sql_dealloc_env(SQLHENV *handle_env)
    {
        handle_odbc_call(*handle_env, SQL_HANDLE_ENV, SQLFreeHandle(SQL_HANDLE_ENV,*handle_env));
    }

    void sql_set_env_attr(SQLHENV *handle_env, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length)
    {
        handle_odbc_call(*handle_env, SQL_HANDLE_ENV, SQLSetEnvAttr(*handle_env, attribute, value_ptr, string_length));
    }

    // --------------- DATABASE CONNECTION RELATED CALLS ---------------------
    void sql_alloc_dbc(SQLHENV *handle_env, SQLHDBC *handle_dbc)
    {
        handle_odbc_call(*handle_env, SQL_HANDLE_ENV, SQLAllocHandle(SQL_HANDLE_DBC, *handle_env, handle_dbc));
    }

    void sql_dealloc_dbc(SQLHDBC *handle_dbc)
    {
        handle_odbc_call(*handle_dbc, SQL_HANDLE_DBC, SQLFreeHandle(SQL_HANDLE_DBC,*handle_dbc));
    }

    void sql_driver_connect(SQLHDBC *handle_dbc, std::string &in_conn_str)
    {
        handle_odbc_call(*handle_dbc,
                         SQL_HANDLE_DBC,
                         SQLDriverConnect(*handle_dbc,
                                          nullptr,
                                          reinterpret_cast< unsigned char * >(in_conn_str.data()),
                                          SQL_NTS,
                                          nullptr,
                                          0,
                                          nullptr,
                                          SQL_DRIVER_NOPROMPT));
    }

}   // namespace aodbc
