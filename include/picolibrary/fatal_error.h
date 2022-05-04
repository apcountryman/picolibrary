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
 * \brief picolibrary fatal error interface.
 */

#ifndef PICOLIBRARY_FATAL_ERROR_H
#define PICOLIBRARY_FATAL_ERROR_H

#include "picolibrary/error.h"

namespace picolibrary {

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

} // namespace picolibrary

#endif // PICOLIBRARY_FATAL_ERROR_H
