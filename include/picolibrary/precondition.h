/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary precondition interface.
 */

#ifndef PICOLIBRARY_PRECONDITION_H
#define PICOLIBRARY_PRECONDITION_H

#include "picolibrary/fatal_error.h"
#include "picolibrary/rom.h"

namespace picolibrary {

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a precondition's expectation.
 *
 * \attention This function should not be used directly. Use PICOLIBRARY_EXPECT() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] expectation The expectation to check.
 * \param[in] file The file in which the check occurs.
 * \param[in] line The line on which the check occurs.
 * \param[in] error The fatal error that occurs if the expectation is not met.
 */
template<typename Error>
constexpr void expect( bool expectation, ROM::String file, int line, Error error ) noexcept
{
    if ( not expectation ) {
        trap_fatal_error( file, line, error );
    } // if
}
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a precondition's expectation.
 *
 * \attention This function should not be used directly. Use PICOLIBRARY_EXPECT() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] expectation The expectation to check.
 * \param[in] error The fatal error that occurs if the expectation is not met.
 */
template<typename Error>
constexpr void expect( bool expectation, Error error ) noexcept
{
    if ( not expectation ) {
        trap_fatal_error( error );
    } // if
}
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a precondition's expectation.
 *
 * \param[in] expectation The expectation to check.
 * \param[in] error The fatal error that occurs if the expectation is not met.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_EXPECT( expectation, error ) \
    ::picolibrary::expect( expectation, PICOLIBRARY_ROM_STRING( __FILE__ ), __LINE__, error )
#else // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Check a precondition's expectation.
 *
 * \param[in] expectation The expectation to check.
 * \param[in] error The fatal error that occurs if the expectation is not met.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_EXPECT( expectation, error ) \
    ::picolibrary::expect( expectation, error )
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a precondition's expectation has not been met.
 *
 * \attention This function should not be used directly. Use
 *            PICOLIBRARY_EXPECTATION_NOT_MET() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] file The file in which the report is made.
 * \param[in] line The line on which the report is made.
 * \param[in] error The fatal error that occurs.
 */
template<typename Error>
[[noreturn]] void expectation_not_met( ROM::String file, int line, Error error ) noexcept
{
    trap_fatal_error( file, line, error );
}
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a precondition's expectation has not been met.
 *
 * \attention This function should not be used directly. Use
 *            PICOLIBRARY_EXPECTATION_NOT_MET() instead.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] error The fatal error that occurs.
 */
template<typename Error>
[[noreturn]] void expectation_not_met( Error error ) noexcept
{
    trap_fatal_error( error );
}
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a precondition's expectation has not been met.
 *
 * \param[in] error The fatal error that occurs.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_EXPECTATION_NOT_MET( error ) \
    ::picolibrary::expectation_not_met( PICOLIBRARY_ROM_STRING( __FILE__ ), __LINE__, error )
#else // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Report that a precondition's expectation has not been met.
 *
 * \param[in] error The fatal error that occurs.
 */
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PICOLIBRARY_EXPECTATION_NOT_MET( error ) \
    ::picolibrary::expectation_not_met( error )
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

/**
 * \brief Bypass precondition expectation checks tag.
 */
struct Bypass_Precondition_Expectation_Checks {
};

/**
 * \brief Bypass precondition expectation checks tag.
 */
constexpr auto BYPASS_PRECONDITION_EXPECTATION_CHECKS = Bypass_Precondition_Expectation_Checks{};

/**
 * \brief Run precondition expectation checks tag.
 */
struct Run_Precondition_Expectation_Checks {
};

/**
 * \brief Run precondition expectation checks tag.
 */
constexpr auto RUN_PRECONDITION_EXPECTATION_CHECKS = Run_Precondition_Expectation_Checks{};

} // namespace picolibrary

#endif // PICOLIBRARY_PRECONDITION_H
