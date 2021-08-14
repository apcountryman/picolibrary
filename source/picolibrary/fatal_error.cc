/**
 * picolibrary
 *
 * Copyright 2020-2021, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::trap_fatal_error() implementation.
 */

#include "picolibrary/fatal_error.h"

#include <cstdlib>

#include "picolibrary/error.h"

namespace picolibrary {

void trap_fatal_error() noexcept
{
    std::abort();
}

void trap_fatal_error( Error_Code const & fatal_error ) noexcept
{
    static_cast<void>( fatal_error );

    std::abort();
}

} // namespace picolibrary
