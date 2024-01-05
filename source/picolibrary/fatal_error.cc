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
 * \brief picolibrary fatal error implementation.
 */

#include "picolibrary/fatal_error.h"

#include <cstdlib>

#include "picolibrary/error.h"
#include "picolibrary/rom.h"

namespace picolibrary {

#ifndef PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
void trap_fatal_error( ROM::String file, int line, Error_Code const & error ) noexcept
{
    static_cast<void>( file );
    static_cast<void>( line );
    static_cast<void>( error );

    std::abort();
}
#else  // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION
void trap_fatal_error( Error_Code const & error ) noexcept
{
    static_cast<void>( error );

    std::abort();
}
#endif // PICOLIBRARY_SUPPRESS_ASSERTION_FAILURE_LOCATION_INFORMATION

} // namespace picolibrary
