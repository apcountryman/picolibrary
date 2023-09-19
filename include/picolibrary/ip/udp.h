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
 * \brief picolibrary::IP::UDP interface.
 */

#ifndef PICOLIBRARY_IP_UDP_H
#define PICOLIBRARY_IP_UDP_H

#include <cstdint>

#include "picolibrary/ip.h"

/**
 * \brief UDP over IP facilities.
 */
namespace picolibrary::IP::UDP {

/**
 * \brief Port.
 */
using Port = ::picolibrary::IP::Port;

/**
 * \brief Endpoint.
 */
using Endpoint = ::picolibrary::IP::Endpoint;

/**
 * \brief Reception result.
 */
struct Reception_Result {
    /**
     * \brief The endpoint the datagram was received from.
     */
    Endpoint endpoint;

    /**
     * \brief The end of the data read from the datagram.
     */
    std::uint8_t * end;
};

} // namespace picolibrary::IP::UDP

#endif // PICOLIBRARY_IP_UDP_H
