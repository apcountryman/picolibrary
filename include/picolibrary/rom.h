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
#endif // __has_include( "picolibrary/hil/rom.h" )

/**
 * \brief Read-Only Memory (ROM) facilities.
 */
namespace picolibrary::ROM {

#ifndef PICOLIBRARY_ROM_STRING_IS_HIL_SPECIFIC
/**
 * \brief A handle to a null-terminated string that can be stored in ROM.
 *
 * To create a string literal that can be placed in ROM, use `PICOLIBRARY_ROM_STRING()`.
 *
 * A HIL can replace this type with a HIL specific version by doing the following:
 * - Configure the `PICOLIBRARY_HIL_INCLUDE_DIR` picolibrary project configuration option
 * - Provide `picolibrary/hil/rom.h`
 * - Define `PICOLIBRARY_ROM_STRING_IS_SPECIALIZED` in `picolibrary/hil/rom.h`
 * - Define the HIL specific version of `::picolibrary::ROM::String` in
 *   `picolibrary/hil/rom.h`
 */
using String = char const *;
#endif // PICOLIBRARY_ROM_STRING_IS_HIL_SPECIFIC

#ifndef PICOLIBRARY_ROM_STRING
/**
 * \brief Create a string literal that can be placed in ROM.
 *
 * \param[in] string The string literal that can be placed in ROM.
 *
 * \return A handle to the string literal.
 *
 * A HIL can replace this macro by doing the following:
 * - Configure the `PICOLIBRARY_HIL_INCLUDE_DIR` picolibrary project configuration option
 * - Provide `picolibrary/hil/rom.h`
 * - Define the macro replacement in `picolibrary/hil/rom.h`
 */
#define PICOLIBRARY_ROM_STRING( string ) ( string )
#endif // PICOLIBRARY_ROM_STRING

} // namespace picolibrary::ROM

#endif // PICOLIBRARY_ROM_H
