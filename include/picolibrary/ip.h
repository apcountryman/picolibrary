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
 * \brief picolibrary::IP interface.
 */

#ifndef PICOLIBRARY_IP_H
#define PICOLIBRARY_IP_H

#include <cstdint>

/**
 * \brief Internet Protocol (IP) facilities.
 */
namespace picolibrary::IP {

/**
 * \brief IP version.
 */
enum class Version : std::uint_fast8_t {
    UNSPECIFIED = 0, ///< Unspecified.
    _4          = 4, ///< IPv4.
};

} // namespace picolibrary::IP

#endif // PICOLIBRARY_IP_H
