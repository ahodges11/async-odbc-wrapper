//
// Created by ahodges on 04/10/2020.
//

#pragma once

#include <stdexcept>

// TODO find out of this can be written with a template class
//#define new_exception(base,child) struct child : base {using base::base;};

namespace aodbc
{
    struct aodbc_base_exception : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    struct aodbc_odbc_exception : aodbc_base_exception
    {
        using aodbc_base_exception::aodbc_base_exception;
    };



}   // namespace aodbc
