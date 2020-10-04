//
// Created by ahodges on 03/10/2020.
//

#pragma once

#include <sql.h>

#include <iostream>
#define log(message) std::cout << message << std::endl;

namespace aodbc
{
    struct connection
    {
        void run()
        {
            // Alloc handle_env and register version
            retcode = SQLAllocEnv(&handle_env);   // TODO could be SQLAllocHandle
            if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO))
            {
                // Error in AllocHandle
                log("")
            }
        }

      private:
        SQLHENV   handle_env;   // Holds all info about ODBC environment.
        SQLHDBC   handle_dbc;
        SQLHSTMT  handle_stmt = 0;
        SQLRETURN retcode;
    };

}   // namespace aodbc