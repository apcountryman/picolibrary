/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::ROM interface.
 */

#ifndef PICOLIBRARY_ROM_H
#define PICOLIBRARY_ROM_H

#include <cstddef>

#if __has_include( "picolibrary/hil/rom.h" )
#include "picolibrary/hil/rom.h"
#endif // __has_include( "picolibrary/hil/rom.h" )

/**
 * \brief ROM facilities.
 */
namespace picolibrary::ROM {

#ifndef PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED
/**
 * \brief A handle to a null-terminated string that may be stored in ROM.
 *
 * To create a string literal that can be stored in ROM, use PICOLIBRARY_ROM_STRING().
 *
 * A HIL can replace this type with a HIL specific version by doing the following:
 * - Provide a picolibrary-hil library that provides picolibrary/hil/rom.h
 * - Define PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED in picolibrary/hil/rom.h
 * - Define the HIL specific version of picolibrary::ROM::String in picolibrary/hil/rom.h
 */
using String = char const *;
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_DEFINED

#ifndef PICOLIBRARY_ROM_STRING
/**
 * \brief Create a string literal that can be stored in ROM.
 *
 * \relatedalso picolibrary::ROM::String
 *
 * \param[in] string The string literal that can be stored in ROM.
 *
 * \return A handle to the string literal that may be stored in ROM.
 *
 * A HIL can replace this macro by doing the following:
 * - Provide a picolibrary-hil library that provides picolibrary/hil/rom.h
 * - Define the macro replacement in picolibrary/hil/rom.h
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_ROM_STRING( string ) ( string )
#endif // PICOLIBRARY_ROM_STRING

/**
 * \brief Get the length of a null-terminated string that may be stored in ROM.
 *
 * \relatedalso picolibrary::ROM::String
 *
 * \param[in] string The null-terminated string that may be stored in ROM to get the
 *            length of.
 *
 * \return The length of the null-terminated string that may be stored in ROM.
 */
inline auto strlen( String string ) noexcept -> std::size_t
{
    auto const begin = string;
    auto       end   = string;

    for ( ; *end; ++end ) {} // for

    return end - begin;
}

} // namespace picolibrary::ROM

#endif // PICOLIBRARY_ROM_H
