//
// Created by ahodges on 07/10/2020.
//

#include "row_set_metadata.hpp"

namespace aodbc::row_set
{

    void generate(column_metadata &metadata,handles::stmt_handle &stmt)
    {
        char        name_buf[1024];
        SQLSMALLINT name_len = 0;

        handle_odbc_call(stmt.get_handle(),
                         SQL_HANDLE_STMT,
                         SQLDescribeCol(stmt.get_handle(),
                                        metadata.number_,
                                        (SQLCHAR *)name_buf,
                                        sizeof(name_buf),
                                        &name_len,
                                        &metadata.sql_type,
                                        &metadata.byte_len,
                                        &metadata.decimal,
                                        &metadata.nullable));
        metadata.name_.append(name_buf, name_len);
    }

    SQLSMALLINT get_column_count(handles::stmt_handle &stmt)
    {
        SQLSMALLINT result;
        handle_odbc_call(stmt.get_handle(), SQL_HANDLE_STMT, SQLNumResultCols(stmt.get_handle(), &result));
        return result;
    }

    void row_set_metadata::build_metadata(handles::stmt_handle &stmt)
    {

        // Get the quantity of columns
        auto column_count = get_column_count(stmt);
        column_metadata_.reserve(column_count);

        for (auto i = 1; i <= column_count; i++)
        {
            generate(column_metadata_.emplace_back(i), stmt);   // Create and fill a column_metadata
        }

    }
}