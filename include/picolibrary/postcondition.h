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
 * \brief picolibrary postcondition interface.
 */

#ifndef PICOLIBRARY_POSTCONDITION_H
#define PICOLIBRARY_POSTCONDITION_H

#include "picolibrary/fatal_error.h"
#include "picolibrary/rom.h"

namespace picolibrary {

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a postcondition's guarantee.
 *
 * \attention This function should not be used directly. Use PICOLIBRARY_ENSURE() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] guarantee The guarantee to check.
 * \param[in] file The file in which the check occurs.
 * \param[in] line The line on which the check occurs.
 * \param[in] error The fatal error that occurs if the guarantee is not met.
 */
template<typename Error>
constexpr void ensure( bool guarantee, ROM::String file, int line, Error error ) noexcept
{
    if ( not guarantee ) {
        trap_fatal_error( file, line, error );
    } // if
}
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a postcondition's guarantee.
 *
 * \attention This function should not be used directly. Use PICOLIBRARY_ENSURE() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] guarantee The guarantee to check.
 * \param[in] error The fatal error that occurs if the guarantee is not met.
 */
template<typename Error>
constexpr void ensure( bool guarantee, Error error ) noexcept
{
    if ( not guarantee ) {
        trap_fatal_error( error );
    } // if
}
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a postcondition's guarantee.
 *
 * \param[in] guarantee The guarantee to check.
 * \param[in] error The fatal error that occurs if the guarantee is not met.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_ENSURE( guarantee, error ) \
    ::picolibrary::ensure( guarantee, __FILE__, __LINE__, error )
#else // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a postcondition's guarantee.
 *
 * \param[in] guarantee The guarantee to check.
 * \param[in] error The fatal error that occurs if the guarantee is not met.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_ENSURE( guarantee, error ) ::picolibrary::ensure( guarantee, error )
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a postcondition's guarantee has not been met.
 *
 * \attention This function should not be used directly. Use
 *            PICOLIBRARY_GUARANTEE_NOT_MET() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] file The file in which the report is made.
 * \param[in] line The line on which the report is made.
 * \param[in] error The fatal error that occurs.
 */
template<typename Error>
[[noreturn]] void guarantee_not_met( ROM::String file, int line, Error error ) noexcept
{
    trap_fatal_error( file, line, error );
}
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a postcondition's guarantee has not been met.
 *
 * \attention This function should not be used directly. Use
 *            PICOLIBRARY_GUARANTEE_NOT_MET() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] error The fatal error that occurs.
 */
template<typename Error>
[[noreturn]] void guarantee_not_met( Error error ) noexcept
{
    trap_fatal_error( error );
}
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a postcondition's guarantee has not been met.
 *
 * \param[in] error The fatal error that occurs.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_GUARANTEE_NOT_MET( error ) \
    ::picolibrary::guarantee_not_met( __FILE__, __LINE__, error )
#else // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a postcondition's guarantee has not been met.
 *
 * \param[in] error The fatal error that occurs.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_GUARANTEE_NOT_MET( error ) ::picolibrary::guarantee_not_met( error )
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

} // namespace picolibrary

#endif // PICOLIBRARY_POSTCONDITION_H
