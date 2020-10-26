//
// Created by ahodges on 06/10/2020.
//

#pragma once

#include "row_set_base.hpp"

#include <cstdint>
#include <memory>

namespace aodbc::row_set
{
    struct field
    {
        field(void *val_ptr)
        : val_ptr_(val_ptr)
        {
        }

        /// @brief Get the underlying value as the specified type.
        /// @tparam value_type The type of the value
        /// @return const ref to the underlying value.
        ///
        /// Note that if the type value_type differs from the actual ODBC type of the column this field is from,
        /// undefined behaviour. Check the column metadata in result_set to find the correct type.
        template < typename value_type >
        const value_type &as()
        {
            return reinterpret_cast< value_type >(val_ptr_);
        }

      private:
        void *val_ptr_;
    };

    struct compact_row_set : row_set_base
    {
        compact_row_set(const row_set_metadata metadata, std::size_t max_row_size = 100000)
        : row_set_base(metadata)
        {
            // Calculate storage heuristics
            data_.resize(1);
            void *dummy_initial = data_.data();
            void *dummy_current = dummy_initial;

            const row_set_metadata &col_meta  = get_metadata();
            auto                    col_count = col_meta.column_count();
            // reserve space
            named_indexes_.reserve(col_count);
            column_indexes_.reserve(col_count);
            for (auto x = 0; x < col_count; x++)
            {
                auto &col = col_meta.get_column_metadata(x);
                // Get the alignment and size of the type of this col_;
                auto [size, alignment] = col.size_and_alignment();

                // align the column
                if (std::align(alignment, size, (void *&)dummy_current, max_row_size))
                {
                    std::size_t tmp_index = (char *)dummy_current - (char *)dummy_initial;
                    column_indexes_.emplace_back(tmp_index);
                    named_indexes_.emplace(col.name(), tmp_index);

                    // Move the current pointer forward the size of the object.
                    dummy_current = (char *)dummy_current + size;
                }
                else
                {
                    // can't fit, max_row_size too small
                    throw std::runtime_error("compact_row_set max_row_size exceeded, could not build heuristics.");
                }
            }

            // Increment the max for size for saftey, as the alignment spacing at the end of the row doesn't count toward it.
            max_row_size += alignof(std::max_align_t);
            // All columns have been aligned.
            // Calculate the space needed before the first column can be aligned correctly, that is the total row length
            auto [size, alignment] = col_meta.get_column_metadata(0).size_and_alignment();
            std::align(alignment, size, (void *&)dummy_current, max_row_size);
            row_size_ = (char *)dummy_current - (char *)dummy_initial;
        }

        bool   fetch_one() override { return false; }
        std::size_t fetch_many() override { return 0; }
        std::size_t fetch_all() override { return 0; }
        std::size_t rows() const override {return data_.size()/row_size_;}

        // field accessor
        // doesn't type check in production
        template<typename FieldType>
        FieldType * get(std::size_t row, std::size_t column_index)
        {
            // TODO check the type against the column index


            return (FieldType*)(row_ptr(row) + column_indexes_[column_index]);
        }

      private:

        std::uint8_t * row_ptr(std::size_t row_number)
        {
            return data_.data() + ( row_number * row_size_);
        }

        // Storage heuristics
        std::size_t                                    row_size_;         // size per row.
        std::vector< std::size_t >                     column_indexes_;   // column indexes into data
        std::unordered_map< std::string, std::size_t > named_indexes_;    // column name to data index for named lookup.

        // Raw row data
        std::vector< std::uint8_t > data_;
    };

}   // namespace aodbc::row_set
