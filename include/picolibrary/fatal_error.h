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
 * \brief picolibrary fatal error interface.
 */

#ifndef PICOLIBRARY_FATAL_ERROR_H
#define PICOLIBRARY_FATAL_ERROR_H

#include "picolibrary/error.h"
#include "picolibrary/rom.h"

namespace picolibrary {

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Trap a fatal error.
 *
 * \attention The picolibrary static library does not provide an implementation for this
 *            function. Library users can:
 *            - Provide an application specific implementation
 *            - Link with the picolibrary-fatal_error static library which provides an
 *              implementation that calls std::abort()
 *            - Link with the picolibrary-testing-automated-fatal_error static library
 *              (only available if automated testing is enabled) which provides an
 *              implementation that writes error information to std::cerr before calling
 *              std::abort()
 *
 * \param[in] file The file in which the fatal error occurred.
 * \param[in] line The line on which the fatal error occurred.
 * \param[in] error The fatal error.
 */
[[noreturn]] void trap_fatal_error( ROM::String file, int line, Error_Code const & error ) noexcept;
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
/**
 * \brief Trap a fatal error.
 *
 * \attention The picolibrary static library does not provide an implementation for this
 *            function. Library users can:
 *            - Provide an application specific implementation
 *            - Link with the picolibrary-fatal_error static library which provides an
 *              implementation that calls std::abort()
 *            - Link with the picolibrary-testing-automated-fatal_error static library
 *              (only available if automated testing is enabled) which provides an
 *              implementation that writes error information to std::cerr before calling
 *              std::abort()
 *
 * \param[in] error The fatal error.
 */
[[noreturn]] void trap_fatal_error( Error_Code const & error ) noexcept;
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

} // namespace picolibrary

#endif // PICOLIBRARY_FATAL_ERROR_H
