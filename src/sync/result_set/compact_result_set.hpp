//
// Created by ahodges on 03/11/2020.
//

#pragma once

#include "compact_row_storage.hpp"
#include "handles.hpp"
#include "metadata.hpp"

namespace aodbc::sync::result_set
{
    struct result_view
    {
        result_view(std::shared_ptr< const compact_row_storage > storage)
        : storage_(std::move(storage))
        , row_count_(storage_->row_count())
        {
        }

        std::size_t row_count() const { return row_count_; }

        aodbc::types::aodbc_bit get_bit(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_bit();
            else
                return aodbc::types::aodbc_bit(*reinterpret_cast< const bool * >(value_ptr));
        }

        aodbc::types::aodbc_bigint get_bigint(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_bigint();
            else
                return aodbc::types::aodbc_bigint(*reinterpret_cast< const std::int64_t * >(value_ptr));
        }
        aodbc::types::aodbc_int get_int(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_int();
            else
                return aodbc::types::aodbc_int(*reinterpret_cast< const std::int32_t * >(value_ptr));
        }
        aodbc::types::aodbc_short get_short(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_short();
            else
                return aodbc::types::aodbc_short(*reinterpret_cast< const std::int16_t * >(value_ptr));
        }
        aodbc::types::aodbc_tinyint get_tinyint(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_tinyint();
            else
                return aodbc::types::aodbc_tinyint(*reinterpret_cast< const std::uint8_t * >(value_ptr));
        }
        aodbc::types::aodbc_string_view get_string_view(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            auto ind                    = indicator;
            boost::ignore_unused(ind);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_string_view();
            else
                return aodbc::types::aodbc_string_view(reinterpret_cast< const char * >(value_ptr));
        }
        aodbc::types::aodbc_float get_float(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_float();
            else
                return aodbc::types::aodbc_float(*reinterpret_cast< const float * >(value_ptr));
        }
        aodbc::types::aodbc_double get_double(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_double();
            else
                return aodbc::types::aodbc_double(*reinterpret_cast< const double * >(value_ptr));
        }
        aodbc::types::aodbc_date get_date(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_date();
            else
                return aodbc::types::aodbc_date(*reinterpret_cast< const types::aodbc_date::type * >(value_ptr));
        }
        aodbc::types::aodbc_time get_time(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_time();
            else
                return aodbc::types::aodbc_time(*reinterpret_cast< const types::aodbc_time::type * >(value_ptr));
        }
        aodbc::types::aodbc_timestamp get_timestamp(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_timestamp();
            else
                return aodbc::types::aodbc_timestamp(
                    *reinterpret_cast< const types::aodbc_timestamp::type * >(value_ptr));
        }
        aodbc::types::aodbc_decimal get_decimal(std::size_t row, std::size_t column) const
        {
            auto [value_ptr, indicator] = storage_->get_value(row, column);
            if (indicator == SQL_NULL_DATA)
                return aodbc::types::aodbc_decimal();
            else
                return aodbc::types::aodbc_decimal(*reinterpret_cast< const types::aodbc_decimal::type * >(value_ptr));
        }

      private:
        std::shared_ptr< const compact_row_storage > storage_;
        const std::size_t                            row_count_;
    };

    struct compact_result_set
    {
        compact_result_set(handles::stmt_handle &stmt)
        : stmt_(stmt)
        , metadata_(std::make_shared< compact_metadata >())
        , storage_()
        {
        }

        void init()
        {
            // generate metadata
            metadata_->generate(stmt_);
            storage_ = std::make_shared< compact_row_storage >(metadata_);
        }

        bool fetch_one() { return fetch_many(1); }

        bool fetch_many(std::size_t quantity)
        {
            // prepare storage area
            auto storage_area = storage_->prepare(quantity);

            //  set SQL_ATTR_ROW_ARRAY_SIZE to the quantity of rows prepared
            sql_set_array_size(stmt_.get_handle(), storage_area.row_count());

            // bind columns
            for (std::size_t col = 0; col < storage_area.column_count(); col++)
            {
                auto sql_col_number = col + 1;
                auto type           = metadata_->get_col(sql_col_number).to_c_type();
                auto size           = metadata_->get_column_size(sql_col_number);

                sql_bind_col(stmt_.get_handle(),
                             sql_col_number,
                             type,
                             storage_area.get_column_ptr(col),
                             size,
                             storage_area.get_indicator_ptr(col));
            }

            SQLROWSETSIZE               rows_returned;
            std::vector< SQLUSMALLINT > status_array;
            status_array.resize(storage_area.row_count());

            // Set the rows returned ptr and status array ptr
            sql_set_rows_fetched_ptr(stmt_.get_handle(), &rows_returned);
            sql_set_row_status_ptr(stmt_.get_handle(), status_array.data());

            // fetch data
            sql_fetch_scroll(stmt_.get_handle(), SQL_FETCH_NEXT, 0);

            if (rows_returned == 0)
            {
                return false;
            }
            std::size_t rows_fetched = 0;
            // check data status array.
            bool had_norow = false;
            for (auto &val : status_array)
            {
                switch (val)
                {
                case SQL_ROW_ERROR:
                    BOOST_LOG_TRIVIAL(error) << "Error while fetching row!";
                    throw std::runtime_error("error in row status array");
                case SQL_ROW_NOROW:
                    // no row here, don't count it.
                    had_norow = true;
                    break;
                case SQL_ROW_SUCCESS_WITH_INFO:
                    // todo get info
                case SQL_ROW_SUCCESS:
                    if (had_norow)
                    {
                        BOOST_LOG_TRIVIAL(error) << "Received a valid row after a SQL_ROW_NOROW code";
                        throw std::runtime_error("Received a valid row after a SQL_ROW_NOROW code");
                    }
                default:
                    rows_fetched++;   // count the row
                }
            }
            assert(rows_fetched == rows_returned);

            // commit the fetch
            storage_->commit(rows_fetched);
            return rows_fetched == quantity;
        }

        void fetch_all(std::size_t rows_per_fetch = 256)
        {
            bool more_rows = true;
            while (more_rows)
            {
                more_rows = fetch_many(rows_per_fetch);
            }
        }

        result_view get_result_view() const { return result_view(storage_); }

        /// @brief Return a reference to the metadata.
        std::shared_ptr< const compact_metadata > get_metadata() const { return metadata_; }

      private:
        handles::stmt_handle                   stmt_;
        std::shared_ptr< compact_metadata >    metadata_;
        std::shared_ptr< compact_row_storage > storage_;
    };

}   // namespace aodbc::sync::result_set