/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Bit_Manipulation interface.
 */

#ifndef PICOLIBRARY_BIT_MANIPULATION_H
#define PICOLIBRARY_BIT_MANIPULATION_H

#include <cstdint>
#include <limits>

namespace picolibrary {

/**
 * \brief Lookup an integer's highest bit set.
 *
 * \tparam Integer The type of integer the lookup will be performed on.
 *
 * \param[in] value The value whose highest bit set is to be looked up.
 *
 * \return The value's highest bit set.
 */
template<typename Integer>
constexpr auto highest_bit_set( Integer value ) noexcept
{
    auto bit = std::uint_fast8_t{ 0 };

    for ( ; value >>= 1; ++bit ) {} // for

    return bit;
}

/**
 * \brief Create a bit mask.
 *
 * \tparam Mask The type of mask to create.
 *
 * \param[in] size The size of the mask (the number of masked bits).
 * \param[in] bit The bit position of the least significant masked bit.
 *
 * \return The created bit mask.
 */
template<typename Mask>
constexpr auto mask( std::uint_fast8_t size, std::uint_fast8_t bit ) noexcept
{
    return static_cast<Mask>(
        ( std::numeric_limits<Mask>::max() >> ( std::numeric_limits<Mask>::digits - size ) ) << bit );
}

/**
 * \brief Reflect the bits in an integer.
 *
 * \tparam Integer The integer type to reflect.
 *
 * \param[in] value The value to reflect.
 *
 * \return The reflected value.
 */
template<typename Integer>
auto reflect( Integer value ) noexcept -> Integer
{
    auto result           = value;
    auto remaining_shifts = std::numeric_limits<Integer>::digits - 1;

    for ( value >>= 1; value; value >>= 1 ) {
        result <<= 1;
        result |= value & 1;

        --remaining_shifts;
    } // for

    return result << remaining_shifts;
}

} // namespace picolibrary

#endif // PICOLIBRARY_BIT_MANIPULATION_H
