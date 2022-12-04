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
 * \brief picolibrary::ROM interface.
 */

#ifndef PICOLIBRARY_ROM_H
#define PICOLIBRARY_ROM_H

#if __has_include( "picolibrary/hil/rom.h" )

#include "picolibrary/hil/rom.h"

/**
 * \brief ROM access is specialized.
 */
#define PICOLIBRARY_SPECIALIZED_ROM_ACCESS

#endif // __has_include( "picolibrary/hil/rom.h" )

/**
 * \brief Read-Only Memory (ROM) facilities.
 */
namespace picolibrary::ROM {

/**
 * \brief A character that can be stored in ROM.
 *
 * To define a string literal that can be placed in ROM, use PICOLIBRARY_ROM_STRING(). If
 * the HIL does not specialize ROM access, the string literal's handle is simply a `char
 * const *` and is accessed accordingly. If the HIL does specialize ROM access, the string
 * literal's handle is a pointer to an incomplete type. To access the contents of the
 * string literal, the string literal's handle must be converted to a usable form by
 * calling `::picolibrary::ROM::reinterpret_string()`. Once the string literal's handle
 * has been converted to a usable form, the handle may be incremented like a normal
 * pointer, or dereferenced by calling `::picolibrary::ROM::dereference_string()`.
 */
#ifndef PICOLIBRARY_SPECIALIZED_ROM_ACCESS
using Character = char;
#else  // PICOLIBRARY_SPECIALIZED_ROM_ACCESS
class Character;
#endif // PICOLIBRARY_SPECIALIZED_ROM_ACCESS

#ifndef PICOLIBRARY_SPECIALIZED_ROM_ACCESS
/**
 * \brief Define a string literal that can be placed in ROM.
 *
 * \param[in] string The string literal that can be placed in ROM.
 *
 * \return A handle to the string literal that can be placed in ROM.
 *
 * If the HIL does not specialize ROM access, the handle to the string literal is simply a
 * `char const *` and is accessed accordingly. If the HIL does specialize ROM access, the
 * handle to the string literal must be a pointer to the `::picolibrary::ROM::Character`
 * incomplete type. To access the contents of the string literal, the handle to the string
 * literal must be converted to a usable form by calling
 * `::picolibrary::ROM::reinterpret_string()`. Once the handle to the string literal has
 * been converted to a usable form, the handle may be incremented like a pointer, or
 * dereferenced by calling `::picolibrary::ROM::dereference_string()`.
 *
 * \code
 * void foo( char const * string )
 * {
 *     while ( auto const character = *string++ ) {
 *         ...
 *     } // while
 * }
 *
 * #ifdef PICOLIBRARY_SPECIALIZED_ROM_ACCESS
 * void foo( ::picolibrary::ROM::Character const * string )
 * {
 *     for ( auto reinterpreted_string = ::picolibrary::ROM::reinterpret_string( string );
 *           auto character = ::picolibrary::ROM::dereference_string( reinterpreted_string );
 *           ++reinterpreted_string ) {
 *         ...
 *     } // for
 * }
 * #endif // PICOLIBRARY_SPECIALIZED_ROM_ACCESS
 *
 * // The `foo( char const * )` overload will be called if the HIL does not specialize ROM
 * // access. The `foo( ::picolibrary::ROM::Character const *` overload will be called if
 * // the HIL does specialize ROM access.
 * foo( PICOLIBRARY_ROM_STRING( "foo" ) );
 * \endcode
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_ROM_STRING( string ) ( string )
#endif // PICOLIBRARY_SPECIALIZED_ROM_ACCESS

#ifndef PICOLIBRARY_SPECIALIZED_ROM_ACCESS
/**
 * \brief Convert the a handle to a string literal that can be stored in ROM to a usable
 *        form.
 *
 * \param[in] string The handle to a string literal that can be stored in ROM to convert
 *            to a usable form.
 *
 * \return The usable form of the handle to the string literal that can be stored in ROM.
 */
constexpr auto reinterpret_string( Character const * string ) noexcept -> char const *
{
    return string;
}
#endif // PICOLIBRARY_SPECIALIZED_ROM_ACCESS

#ifndef PICOLIBRARY_SPECIALIZED_ROM_ACCESS
/**
 * \brief Dereference a handle to a string literal that can be stored in ROM.
 *
 * \param[in] string The usable form of a handle to a string literal that can be stored in
 *            ROM to dereference.
 *
 * \return The result of the dereference.
 */
constexpr auto dereference_string( char const * string ) noexcept -> char
{
    return *string;
}
#endif // PICOLIBRARY_SPECIALIZED_ROM_ACCESS

} // namespace picolibrary::ROM

#endif // PICOLIBRARY_ROM_H
