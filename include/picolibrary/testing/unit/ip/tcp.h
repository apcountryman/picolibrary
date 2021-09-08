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
 * \brief picolibrary::Testing::Unit::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_IP_TCP_H
#define PICOLIBRARY_TESTING_UNIT_IP_TCP_H

#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/testing/unit/ip.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random TCP over IP port number.
 *
 * \return A pseudo-random TCP over IP port number.
 */
template<>
inline auto random<::picolibrary::IP::TCP::Port>()
{
    return ::picolibrary::IP::TCP::Port{ random<::picolibrary::IP::TCP::Port::Unsigned_Integer>() };
}

/**
 * \brief Generate a pseudo-random TCP over IP endpoint.
 *
 * \brief A pseudo-random TCP over IP endpoint.
 */
template<>
inline auto random<::picolibrary::IP::TCP::Endpoint>()
{
    return ::picolibrary::IP::TCP::Endpoint{ random<::picolibrary::IP::Address>(),
                                             random<::picolibrary::IP::TCP::Port>() };
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Transmission Control Protocol (TCP) over IP unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IP::TCP {
} // namespace picolibrary::Testing::Unit::IP::TCP

#endif // PICOLIBRARY_TESTING_UNIT_IP_TCP_H
