//
// Created by ahodges on 03/11/2020.
//
#pragma once

#include <exception>
#include <iostream>
#include <string>

void exception_handler(std::exception_ptr ptr, std::string origin)
{
    if (ptr)
    {
        try
        {
            std::rethrow_exception(ptr);
        }
        catch (std::exception &ex)
        {
            std::cout << "exception thrown from: " << origin << std::endl;
            std::cout << "exception: " << ex.what() << std::endl;
        }
    }
}