//
// Created by ahodges on 06/11/2020.
//

#pragma once

#include <AODBC/sync/result_set/compact_metadata.hpp>
#include <AODBC/config.hpp>

#include <boost/core/ignore_unused.hpp>
#include <deque>
#include <map>
#include <vector>

namespace aodbc::sync::result_set
{
    namespace detail
    {
        struct compact_chunk;
    }

    struct compact_array_pointers
    {
        friend detail::compact_chunk;

        compact_array_pointers(std::size_t column_count, std::size_t row_count)
        : row_count_(row_count)
        , data_ptrs_(column_count)
        , indicator_ptrs_(column_count)
        {
        }

        std::size_t column_count() const { return data_ptrs_.size(); }
        std::size_t row_count() const { return row_count_; }

        char *  get_column_ptr(std::size_t column_number) { return data_ptrs_[column_number]; }
        SQLLEN *get_indicator_ptr(std::size_t column_number) { return indicator_ptrs_[column_number]; }

      private:
        std::size_t             row_count_;
        std::vector< char * >   data_ptrs_;
        std::vector< SQLLEN * > indicator_ptrs_;
    };

    namespace detail
    {
        inline std::size_t calc_chunk_size(std::size_t suggested_size, std::size_t min_size)
        {
            return suggested_size < min_size ? min_size : suggested_size;
        }

        struct compact_chunk
        {
            static const std::size_t DEFAULT_CHUNK_SIZE = 256;

            compact_chunk(const compact_metadata &metadata, std::size_t chunk_size = DEFAULT_CHUNK_SIZE)
            : metadata_(metadata)
            , CHUNK_SIZE(calc_chunk_size(chunk_size, DEFAULT_CHUNK_SIZE))
            , row_count_(0)
            , data_(metadata_.calculate_data_size(CHUNK_SIZE))
            , indicators_(metadata_.column_count() * CHUNK_SIZE)
            , prepared_(false)
            {
                // generate column_info/indicator_info
                std::size_t col_bytes_in  = 0;
                std::size_t ind_values_in = 0;
                for (std::size_t x = 1; x <= metadata_.column_count(); x++)
                {
                    auto column_size = metadata_.get_column_size(x);
                    column_info.emplace_back(col_bytes_in, column_size);
                    col_bytes_in += column_size * CHUNK_SIZE;

                    indicator_info.emplace_back(ind_values_in);
                    ind_values_in += CHUNK_SIZE;
                }
            }

            /// @brief Query the remaining quantity of rows that can be stored in this chunk.
            /// @return Remaining quantity of rows that can be stored.
            std::size_t remaining_space() const { return CHUNK_SIZE - row_count_; }

            /// @brief Prepare array space for `row_count` rows.
            ///
            /// @attention A call to `commit()` must be made before a subsequent call to prepare is made.
            ///
            /// @param row_count The quantity of rows wanting to be stored.
            /// @warning `row_count` cannot exceed `remaining_space()` or undefined.
            ///
            /// @return Two vectors ot pointers to storage arrays, first for values, second for indicators.
            /// Each storage array is of length row_count. Using space before or after the buffer is undefined.
            compact_array_pointers prepare(std::size_t row_count)
            {
                assert(row_count <= remaining_space());
                assert(not prepared_);
                prepared_ = true;

                compact_array_pointers result(column_info.size(), row_count);

                // calculate the ptr to all columns
                for (std::size_t x = 0; x < column_info.size(); x++)
                {
                    {   // value buffer
                        auto [begin_index, element_size] = column_info[x];
                        // calculate where the start of the free space begins
                        result.data_ptrs_[x] = data_.data() + (begin_index + (row_count_ * element_size));
                    }

                    {   // indicator buffer
                        auto begin_index = indicator_info[x];
                        // Don't need to multiply row_count by sizeof(SQLLEN) because the pointer is of type SQLLEN
                        // this is the distance moved automatically.
                        result.indicator_ptrs_[x] = indicators_.data() + (begin_index + row_count_);
                    }
                }
                return result;
            }

            /// @brief Confirms how many of the prepared rows were consumed.
            ///
            /// @param quantity The quantity of rows consumed
            /// @warning If quantity is greater than the quantity of prepared rows, undefined behaviour.
            ///
            /// @return true if there is space remaining in the chunk, false if full.
            bool commit(std::size_t quantity)
            {
                assert(prepared_);
                assert(quantity < remaining_space());
                prepared_ = false;
                row_count_ += quantity;
                return remaining_space();
            }

            /// @brief Get the value and indicator info for that value for a given row and column.
            /// @param row The row index of the value (starts at 1)
            /// @param column The column index of the value (starts at 1)
            /// @return Tuple containing: the  and indicator info
            /// [0] - pointer to the start of the values data
            /// [1] - value indicator info
            std::tuple< const void *, SQLLEN > get_value(std::size_t row, std::size_t column)
            {
                assert(row <= row_count_);
                assert(column <= column_info.size());

                // get value
                auto [column_index, value_size] = column_info[column - 1];
                auto column_ind                 = column_index;
                auto value_si                   = value_size;
                boost::ignore_unused(column_ind, value_si);
                char *start_of_column = data_.data() + column_index;
                char *start_of_value  = start_of_column + ((row - 1) * value_size);

                // get indicator
                auto    begin_index         = indicator_info[column - 1];
                SQLLEN *ind_start_of_column = indicators_.data() + begin_index;
                SQLLEN *ind_start_of_value  = ind_start_of_column + (row - 1);

                const void *val     = reinterpret_cast< const void * >(start_of_value);
                SQLLEN      ind_val = *ind_start_of_value;
                return std::make_tuple(val, ind_val);
            }

          private:
            const compact_metadata &metadata_;
            const std::size_t       CHUNK_SIZE;
            std::size_t             row_count_;
            std::vector< char >     data_;         // data for all columns/fields
            std::vector< SQLLEN >   indicators_;   // size/null info for all fields

            /// [0] = Index to the start the column's data in the buffer as bytes.
            /// [1] = size of one element in bytes
            std::vector< std::tuple< std::size_t, std::size_t > > column_info;
            std::vector< std::size_t >                            indicator_info;
            bool                                                  prepared_;
        };

        struct range
        {
            range(std::size_t first_row, std::size_t length, compact_chunk *chunk)
            : first_row(first_row)
            , length(length)
            , chunk(chunk)
            {
            }

            bool contains(std::size_t row) const { return row >= first_row && row < first_row + length; }

            std::size_t    first_row;
            std::size_t    length;
            compact_chunk *chunk;
        };

        struct range_less
        {
            bool operator()(const range &first, const range &last) const { return first.first_row < last.first_row; }

            bool operator()(std::size_t row, const range &r) const { return row < r.first_row; }

            bool operator()(const range &r, std::size_t row) const { return r.first_row + r.length <= row; }

            struct is_transparent
            {
            };
        };

    }   // namespace detail

    struct compact_row_storage
    {
        compact_row_storage(std::shared_ptr< compact_metadata > metadata)
        : row_count_(0)
        , metadata_(std::move(metadata))
        , chunk_store_({ *metadata_ })
        , chunk_index_({ { 1, detail::compact_chunk::DEFAULT_CHUNK_SIZE, get_last_chunk() } })
        {
        }

        std::size_t row_count() const { return row_count_; }

        /// @brief Prepares array buffers to store data. The buffer space prepared is not guaranteed to be `row_count`
        /// size. Check the actual size prepared by calling `compact_array_pointers.row_count()`
        /// @param row_count The maximum quantity of rows to prepare
        /// @return `compact_array_pointers` The prepared buffers
        compact_array_pointers prepare(std::size_t row_count)
        {
            // Check how many we can store in the last chunk
            std::size_t prepare_count;

            detail::compact_chunk *last_chunk = get_last_chunk();
            if (last_chunk->remaining_space() == 0)
            {
                last_chunk = create_chunk(row_count);   // allocate a new chunk
            }

            // chunk is now guaranteed to have some space in it.
            if (last_chunk->remaining_space() < row_count)
                prepare_count = last_chunk->remaining_space();
            else
                prepare_count = row_count;

            return last_chunk->prepare(prepare_count);
        }

        void commit(std::size_t row_count)
        {
            auto *last_chunk = get_last_chunk();
            last_chunk->commit(row_count);
            row_count_ += row_count;
        }

        /// @brief Drops all stored rows and (most) allocated space, resetting the object to a fresh state.
        void drop()
        {
            row_count_ = 0;
            chunk_store_.clear();
            chunk_store_.emplace_back(*metadata_);
            chunk_index_.clear();
            chunk_index_.emplace_back(1, detail::compact_chunk::DEFAULT_CHUNK_SIZE, get_last_chunk());
        }

      public:   // data accessor functions
        const std::tuple< const void *, SQLLEN > get_value(std::size_t row, std::size_t column) const
        {
            assert(row <= row_count_);
            assert(column <= metadata_->column_count());
            auto [chunk, first_row] = find_chunk(row);
            assert(chunk != nullptr);
            return chunk->get_value((row - first_row) + 1, column);
        }

      private:
        std::tuple< detail::compact_chunk *, std::size_t > find_chunk(std::size_t row) const
        {
            auto ifind = std::lower_bound(chunk_index_.begin(), chunk_index_.end(), row, detail::range_less());
            if (std::end(chunk_index_) == ifind)
                return std::make_tuple(nullptr, 0);
            else
            {
                return ifind->contains(row) ? std::make_tuple(ifind->chunk, ifind->first_row)
                                            : std::make_tuple(nullptr, 0);
            }
        }
        detail::compact_chunk *get_last_chunk() { return std::addressof(chunk_store_.back()); }
        detail::compact_chunk *create_chunk(std::size_t chunk_size)
        {
            // Note that chunk_size is only a minimum. The chunk could have over-allocated.
            chunk_store_.emplace_back(*metadata_, chunk_size);
            // Check the size that was actually allocated
            auto size = get_last_chunk()->remaining_space();
            // update the index, one past the current row_count which is where the next chunk starts
            chunk_index_.emplace_back(row_count_ + 1, size, get_last_chunk());
            return get_last_chunk();
        }

        std::size_t                         row_count_;
        std::shared_ptr< compact_metadata > metadata_;
        std::deque< detail::compact_chunk > chunk_store_;
        std::vector< detail::range >        chunk_index_;
    };

}   // namespace aodbc::sync::result_set
