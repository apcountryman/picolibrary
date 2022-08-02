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
 * \brief picolibrary::WIZnet::W5500::IP interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_H
#define PICOLIBRARY_WIZNET_W5500_IP_H

#include "picolibrary/ip.h"

/**
 * \brief WIZnet W5500 Internet Protocol (IP) facilities.
 */
namespace picolibrary::WIZnet::W5500::IP {

/**
 * \brief Protocol port allocator concept.
 */
class Port_Allocator_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Port_Allocator_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Port_Allocator_Concept( Port_Allocator_Concept && source ) noexcept;

    Port_Allocator_Concept( Port_Allocator_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Port_Allocator_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Port_Allocator_Concept && expression ) noexcept -> Port_Allocator_Concept &;

    auto operator=( Port_Allocator_Concept const & ) = delete;

    /**
     * \brief Allocate a port.
     *
     * \tparam Driver The type of driver used to interact with the W5500.
     *
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] port The port to allocate.
     *
     * \pre port is not already in use
     * \pre if an ephemeral port is requested, an ephemeral port is available
     *
     * \return The allocated port.
     */
    template<typename Driver>
    auto allocate( Driver const & driver, ::picolibrary::IP::Port port ) noexcept
        -> ::picolibrary::IP::Port;

    /**
     * \brief Deallocate a previously allocated port.
     *
     * \param[in] port The previously allocated port to deallocate.
     */
    void deallocate( ::picolibrary::IP::Port port ) noexcept;
};

} // namespace picolibrary::WIZnet::W5500::IP

#endif // PICOLIBRARY_WIZNET_W5500_IP_H
