//
// Created by ahodges on 16/11/2020.
//

#pragma once

#include <cstddef>
#include <map>
#include <vector>
#include <cassert>

namespace aodbc::sync::result_set
{
    struct range_less;

    struct range
    {
        friend range_less;

        bool contains(std::size_t row) const { return row >= first_row && row < first_row + length; }

        std::size_t first_row;
        std::size_t length;
    };

    struct range_less
    {
        bool operator()(const range &first, const range &last) const { return first.first_row < last.first_row; }

        bool operator()(std::size_t row, const range &r) const { return row < r.first_row; }

        bool operator()(const range &r, std::size_t row) const { return r.first_row < row; }

        struct is_transparent
        {
        };
    };

    struct chunk
    {

    };

    struct test
    {
        chunk *get_chunk(std::size_t row)
        {
            auto ifind = m.lower_bound(row);
            if (ifind == std::end(m))
            {
                // didn't find
                return nullptr;
            }
            else
            {
                return ifind->first.contains(row) ? std::addressof(ifind->second) : nullptr;
            }
        }

        std::map< range, chunk, range_less > m;
    };


    struct test_vec
    {

        void append(range r)
        {
            v.push_back(r);
            assert(std::is_sorted(v.begin(),v.end(),range_less()));
        }

        chunk * get_chunk(std::size_t row)
        {
            auto ifind = std::lower_bound(v.begin(),v.end(),row,range_less());
            if (std::end(v) == ifind)
                return nullptr;
            else
            {
                return ifind->contains(row) ? ifind->chunk() : nullptr;
            }
        }



        std::vector<range> v;
    };

}   // namespace aodbc::sync::result_set