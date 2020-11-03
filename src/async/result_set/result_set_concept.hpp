//
// Created by ahodges on 02/11/2020.
//

#pragma once

#include "config.hpp"

#include "sync/result_set/metadata.hpp"

namespace aodbc::async::result_set
{
    template <typename T, typename U>
    concept SameAs = std::is_same_v<T, U>;

    template < typename T >
    concept AsyncResultSet = requires(T &x)
    {
        { x.get_metadata() }->SameAs< const sync::result_set::result_set_metadata & >;
    };


}   // namespace aodbc::async::result_set
