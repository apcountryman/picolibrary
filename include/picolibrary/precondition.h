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
 * \brief picolibrary precondition interface.
 */

#ifndef PICOLIBRARY_PRECONDITION_H
#define PICOLIBRARY_PRECONDITION_H

#include "picolibrary/fatal_error.h"

namespace picolibrary {

/**
 * \brief Check a precondition's expectation.
 *
 * \tparam Error A type that can be implicitly converted to picolibrary::Error_Code.
 *
 * \param[in] expectation The expectation to check.
 * \param[in] error The fatal error that occurs if the expectation is not met.
 */
template<typename Error>
void expect( bool expectation, Error error ) noexcept
{
    if ( not expectation ) {
        trap_fatal_error( error );
    } // if
}

} // namespace picolibrary

#endif // PICOLIBRARY_PRECONDITION_H
