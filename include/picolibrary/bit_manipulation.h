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
constexpr auto highest_bit_set( Integer value )
{
    auto bit = std::uint_fast8_t{ 0 };

    for ( ; value >>= 1; ++bit ) {} // for

    return bit;
}

} // namespace picolibrary

#endif // PICOLIBRARY_BIT_MANIPULATION_H
