//
// Created by ahodges on 06/10/2020.
//

#pragma once

namespace aodbc::row_set
{
    /// @brief A result_set which uses a variant to store individual data fields.
    /// Due to the nature of how a variant is structured, there is a lot of wasted space when storing small data fields
    /// like int,double etc.
    ///
    /// Consider using compact_row_set if a more space efficient solution is required.
    struct variant_row_set
    {



      private:

    };
}   // namespace aodbc::row_set
