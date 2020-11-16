/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * \file
 * \brief picolibrary::Format interface.
 */

#ifndef PICOLIBRARY_FORMAT_H
#define PICOLIBRARY_FORMAT_H

#include <type_traits>

#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/utility.h"

/**
 * \brief Formatting facilities.
 */
namespace picolibrary::Format {

/**
 * \brief Integer binary output format specifier.
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Binary {
  public:
    static_assert( std::is_integral_v<Integer> );

    Binary() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Binary( Binary && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Binary( Binary const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Binary() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Binary && expression ) noexcept -> Binary & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Binary const & expression ) noexcept -> Binary & = default;
};

} // namespace picolibrary::Format

namespace picolibrary {

/**
 * \brief Integer binary output formatter.
 *
 * picolibrary::Format::Binary only supports the default format specification ("{}").
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class Output_Formatter<Format::Binary<Integer>> {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Output_Formatter() noexcept = default;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter && ) = delete;

    /**
     * \todo #29
     */
    Output_Formatter( Output_Formatter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Output_Formatter() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Output_Formatter const & ) = delete;

    /**
     * \brief Parse the format specification for the integer to be formatted.
     *
     * \param[in] format The format specification for the integer to be formatted.
     *
     * \return format.
     */
    constexpr auto parse( char const * format ) noexcept -> Result<char const *, Void>
    {
        return format;
    }

    /**
     * \brief Write the integer to the stream.
     *
     * \param[in] stream The stream to write the integer to.
     * \param[in] integer The integer to write to the stream.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto print( Output_Stream & stream, Format::Binary<Integer> integer ) noexcept
        -> Result<Void, Error_Code>
    {
        static_cast<void>( stream );
        static_cast<void>( integer );

        return {};
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_FORMAT_H
