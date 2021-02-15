//
// Created by ahodges on 06/11/2020.
//

#pragma once

#include <AODBC/sync/result_set/metadata.hpp>

#include <vector>

namespace aodbc::sync::result_set
{
    struct compact_metadata
    {
        void generate(handles::stmt_handle &stmt)
        {
            impl_.generate_column_metadata(stmt);
            generate_column_sizes();
        }

        /// @brief Calculates the size in bytes of `quantity` rows. I.E. the space required to store.
        /// @param quantity The quantity of rows
        /// @return The quantity of bytes.
        std::size_t calculate_data_size(std::size_t quantity) const
        {
            std::size_t total = 0;
            for (auto size : column_sizes_)
            {
                total += size * quantity;
            }
            return total;
        }

        std::size_t column_count() const { return column_sizes_.size(); }

        std::size_t get_column_size(std::size_t column) const
        {
            assert(column <= column_sizes_.size());
            return column_sizes_[column - 1];
        }

        const column_metadata &get_col(std::size_t column_number) const { return impl_.get_col(column_number); }

      private:
        void generate_column_sizes()
        {
            column_sizes_.reserve(impl_.get_column_metadata().size());
            for (auto &col : impl_.get_column_metadata())
            {
                switch (col.sql_type_ident())
                {
                case SQL_CHAR:
                    column_sizes_.emplace_back(1);
                    break;
                case SQL_VARCHAR:
                case SQL_BINARY:
                    column_sizes_.emplace_back(col.byte_len() + 1);   // additional byte for null character
                    break;
                default:
                    column_sizes_.emplace_back(std::get< 0 >(col.c_size_and_alignment()));
                    break;
                }
            }
        }

        std::vector< std::size_t > column_sizes_;   // size of the column type in bytes
        result_set_metadata        impl_;
    };
}   // namespace aodbc::sync::result_set