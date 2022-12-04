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
} // namespace picolibrary::ROM

#endif // PICOLIBRARY_ROM_H
