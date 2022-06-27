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
 * \brief picolibrary utility interface.
 */

#ifndef PICOLIBRARY_UTILITY_H
#define PICOLIBRARY_UTILITY_H

#include <cstring>
#include <type_traits>

namespace picolibrary {

/**
 * \brief Convert an enum value to its underlying type.
 *
 * \tparam Enum The type of enum to convert.
 *
 * \param[in] value The enum value to convert.
 *
 * \return The enum value converted to its underlying type.
 */
template<typename Enum>
constexpr auto to_underlying( Enum value ) noexcept
{
    static_assert( std::is_enum_v<Enum> );

    return static_cast<std::underlying_type_t<Enum>>( value );
}

/**
 * \brief Convert a potentially signed integer its bitwise unsigned integer equivalent.
 *
 * \param[in] integer The potentially signed integer to convert.
 *
 * \return The potentially signed integer converted to its bitwise unsigned integer
 *         equivalent.
 */
template<typename Integer>
constexpr auto to_unsigned( Integer integer ) noexcept
{
    std::make_unsigned_t<Integer> unsigned_integer;
    static_assert( sizeof( unsigned_integer ) == sizeof( integer ) );
    std::memcpy( &unsigned_integer, &integer, sizeof( unsigned_integer ) );
    return unsigned_integer;
}

} // namespace picolibrary

#endif // PICOLIBRARY_UTILITY_H
