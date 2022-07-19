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
 * \brief picolibrary::Testing::Automated::IP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_H

#include "picolibrary/ip.h"
#include "picolibrary/testing/automated/random.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::IP::Version.
 *
 * \return A pseudo-randomly generated picolibrary::IP::Version.
 */
template<>
inline auto random<IP::Version>() -> IP::Version
{
    return random<bool>() ? IP::Version::UNSPECIFIED : IP::Version::_4;
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief Internet Protocol (IP) automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IP {
} // namespace picolibrary::Testing::Automated::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_H
