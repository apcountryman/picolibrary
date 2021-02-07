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

#include <limits>

namespace picolibrary {

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
