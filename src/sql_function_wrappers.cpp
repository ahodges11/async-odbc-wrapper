//
// Created by ahodges on 07/10/2020.
//

#include "sql_function_wrappers.hpp"

#include "log.hpp"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace aodbc
{
    namespace detail
    {
        void handle_diagnostic_messages(SQLHANDLE &             handle,
                                        SQLSMALLINT             handle_type,
                                        std::vector< message > *messages)
        {


            if (messages == nullptr)
                return;
            else
            {
                SQLSMALLINT record_num = 0;
                SQLINTEGER  error_num;
                char        txt[1000];
                SQLCHAR     state[SQL_SQLSTATE_SIZE + 1];

                while (true)
                {
                    auto continue_code = SQLGetDiagRec(handle_type,
                                                       handle,
                                                       ++record_num,
                                                       state,
                                                       &error_num,
                                                       reinterpret_cast< SQLCHAR * >(txt),
                                                       (SQLSMALLINT)(sizeof(txt) / sizeof(WCHAR)),
                                                       (SQLSMALLINT *)nullptr);
                    if (continue_code != SQL_SUCCESS && continue_code != SQL_SUCCESS_WITH_INFO)
                        return;
                    message &msg     = messages->emplace_back();
                    msg.error_number = error_num;
                    std::copy(std::begin(state), std::end(state), msg.state);
                    msg.text = txt;
                }
            }
        }
        void handle_diagnostic_print(SQLHANDLE &handle, SQLSMALLINT handle_type)
        {


            SQLSMALLINT record_num = 0;
            SQLINTEGER  error_num;
            SQLCHAR     txt[1000];
            SQLCHAR     state[SQL_SQLSTATE_SIZE + 1];

            while (true)
            {
                auto continue_code = SQLGetDiagRec(handle_type,
                                                   handle,
                                                   ++record_num,
                                                   state,
                                                   &error_num,
                                                   txt,
                                                   (SQLSMALLINT)(sizeof(txt) / sizeof(WCHAR)),
                                                   (SQLSMALLINT *)nullptr);
                if (continue_code != SQL_SUCCESS && continue_code != SQL_SUCCESS_WITH_INFO)
                    return;
                // print
                log_info(fmt::format("[{}]-[{}]: {}", state, error_num, txt).data());
            }
        }
    }   // namespace detail

    void
    handle_diagnostic(SQLHANDLE &handle, SQLSMALLINT handle_type, std::vector< message > *messages)
    {
        detail::handle_diagnostic_messages(handle, handle_type, messages);
    }
    void handle_diagnostic(SQLHANDLE &handle, SQLSMALLINT handle_type)
    {
        detail::handle_diagnostic_print(handle, handle_type);
    }

    void handle_odbc_call(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode)
    {

        switch (retcode)
        {
        case SQL_SUCCESS:
            return;
        case SQL_SUCCESS_WITH_INFO:
            handle_diagnostic(handle, handle_type);
            return;
        case SQL_ERROR:
            if (handle == nullptr)
            {
                throw std::runtime_error("nullptr handle returned");
            }
            handle_diagnostic(handle, handle_type);
            return;
        case SQL_INVALID_HANDLE:
            throw aodbc_odbc_exception("Invalid handle");
        }
    }
    void handle_odbc_call(SQLHANDLE &handle, SQLSMALLINT handle_type, RETCODE retcode, std::vector< message > *messages)
    {
        switch (retcode)
        {
        case SQL_SUCCESS:
            return;
        case SQL_SUCCESS_WITH_INFO:
            handle_diagnostic(handle, handle_type, messages);
            return;
        case SQL_ERROR:
            handle_diagnostic(handle, handle_type, messages);
            return;
        case SQL_INVALID_HANDLE:
            throw aodbc_odbc_exception("Invalid handle");
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


    void sql_set_env_attr(SQLHENV &handle_env, SQLINTEGER attribute, SQLPOINTER value_ptr, SQLINTEGER string_length)
    {
        handle_odbc_call(handle_env, SQL_HANDLE_ENV, SQLSetEnvAttr(handle_env, attribute, value_ptr, string_length));
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
    void sql_driver_connect(SQLHDBC &handle_dbc, std::string &in_conn_str, std::vector< message > *messages)
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
                                          SQL_DRIVER_NOPROMPT),
                         messages);
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
    void sql_exec_direct(SQLHSTMT &handle_stmt, std::string &statement, std::vector<message> * messages)
    {
        if (messages)
        {
            handle_odbc_call(
                handle_stmt,
                SQL_HANDLE_STMT,
                SQLExecDirect(handle_stmt, reinterpret_cast< unsigned char * >(statement.data()), statement.size()),messages);
        }
        else
        {
            handle_odbc_call(
                handle_stmt,
                SQL_HANDLE_STMT,
                SQLExecDirect(handle_stmt, reinterpret_cast< unsigned char * >(statement.data()), statement.size()));
        }

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
                         SQLSetStmtAttr(handle_stmt, SQL_ATTR_MAX_ROWS, (SQLPOINTER)row_size, SQL_IS_UINTEGER));
    }

    SQLLEN sql_get_row_size(SQLHSTMT &handle_stmt)
    {
        SQLLEN result;
        handle_odbc_call(
            handle_stmt,
            SQL_HANDLE_STMT,
            SQLGetStmtAttr(handle_stmt, SQL_ATTR_MAX_ROWS, reinterpret_cast< SQLPOINTER >(&result), 0, nullptr));
        return result;
    }
    void sql_set_array_size(SQLHSTMT &handle_stmt, SQLULEN array_size)
    {
        handle_odbc_call(handle_stmt,
                         SQL_HANDLE_STMT,
                         SQLSetStmtAttr(handle_stmt,
                                        SQL_ATTR_ROW_ARRAY_SIZE,
                                        reinterpret_cast< SQLPOINTER >(static_cast< uintptr_t >(array_size)),
                                        SQL_IS_UINTEGER));
    }
    void sql_fetch_scroll(SQLHSTMT &handle_stmt, SQLSMALLINT orientation, SQLROWOFFSET offset)
    {
        handle_odbc_call(handle_stmt, SQL_HANDLE_STMT, SQLFetchScroll(handle_stmt, orientation, offset));
    }
    void sql_set_row_status_ptr(SQLHSTMT &handle_stmt, SQLUSMALLINT *row_status)
    {
        handle_odbc_call(
            handle_stmt, SQL_HANDLE_STMT, SQLSetStmtAttr(handle_stmt, SQL_ATTR_ROW_STATUS_PTR, row_status, 0));
    }
    void sql_set_rows_fetched_ptr(SQLHSTMT &handle_stmt, SQLROWSETSIZE *row_count)
    {
        handle_odbc_call(
            handle_stmt, SQL_HANDLE_STMT, SQLSetStmtAttr(handle_stmt, SQL_ATTR_ROWS_FETCHED_PTR, row_count, 0));
    }

    void sql_bind_col(SQLHSTMT &   handle_stmt,
                      SQLUSMALLINT col_number,
                      SQLSMALLINT  col_type,
                      SQLPOINTER   data_ptr,
                      SQLLEN       buf_len,
                      SQLLEN *     indicator_ptr)
    {
        handle_odbc_call(handle_stmt,
                         SQL_HANDLE_STMT,
                         SQLBindCol(handle_stmt, col_number, col_type, data_ptr, buf_len, indicator_ptr));
    }

}   // namespace aodbc