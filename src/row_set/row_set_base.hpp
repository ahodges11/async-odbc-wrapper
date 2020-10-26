//
// Created by ahodges on 06/10/2020.
//


#pragma once

#include "row_set_metadata.hpp"

namespace aodbc::row_set
{

    struct row_set_base
    {
        row_set_base(const row_set_metadata metadata)
        :metadata_(metadata)
        {}

        // Fetch interface, all row_sets should have this functionality.
        virtual bool fetch_one() = 0;
        virtual std::size_t fetch_many() = 0;
        virtual std::size_t fetch_all() = 0;
        virtual std::size_t rows() const = 0;

        const row_set_metadata & get_metadata() const {return metadata_;}
      private:
        const row_set_metadata metadata_;

    };



}
