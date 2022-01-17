/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
 * contributors
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
 * \brief picolibrary bit manipulation interface.
 */

#ifndef PICOLIBRARY_BIT_MANIPULATION_H
#define PICOLIBRARY_BIT_MANIPULATION_H

#include <cstdint>
#include <limits>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Lookup an unsigned integer's highest bit set.
 *
 * \tparam T The type of unsigned integer the lookup will be performed on.
 *
 * \param[in] value The value whose highest bit set is to be looked up (assumed to be
 *            non-zero).
 *
 * \return The value's highest bit set.
 */
template<typename T>
constexpr auto highest_bit_set( T value )
{
    static_assert( std::is_unsigned_v<T> );

    auto bit = std::uint_fast8_t{ 0 };

    for ( ; value >>= 1; ++bit ) {} // for

    return bit;
}

/**
 * \brief Create a bit mask.
 *
 * \tparam T The type of unsigned integer to use for the mask.
 *
 * \param[in] size The size of the mask (the number of masked bits).
 * \param[in] bit The bit position of the least significant masked bit.
 *
 * \return The created bit mask.
 */
template<typename T>
constexpr auto mask( std::uint_fast8_t size, std::uint_fast8_t bit ) noexcept
{
    static_assert( std::is_unsigned_v<T> );

    return static_cast<T>(
        ( static_cast<std::uintmax_t>( std::numeric_limits<T>::max() ) >> ( std::numeric_limits<T>::digits - size ) )
        << bit );
}

/**
 * \brief Reflect the bits in an unsigned integer.
 *
 * \tparam T The unsigned integer type to reflect.
 *
 * \param[in] value The value to reflect.
 *
 * \return The reflected value.
 */
template<typename T>
auto reflect( T value ) noexcept
{
    static_assert( std::is_unsigned_v<T> );

    auto result           = value;
    auto remaining_shifts = std::numeric_limits<T>::digits - 1;

    for ( value >>= 1; value; value >>= 1, --remaining_shifts ) {
        result <<= 1;
        result |= value & 0b1;
    } // for

    return static_cast<T>( result << remaining_shifts );
}

} // namespace picolibrary

#endif // PICOLIBRARY_BIT_MANIPULATION_H
