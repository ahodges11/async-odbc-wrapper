//
// Created by ahodges on 19/11/2020.
//

#pragma once

#include <AODBC/config.hpp>
#include <thread>

namespace aodbc::async
{
    inline net::thread_pool & get_thread_pool()
    {
        //static net::thread_pool p(std::max(std::thread::hardware_concurrency(),1u));
        static net::thread_pool p(std::max(std::thread::hardware_concurrency(),50u));
        return p;
    }
}