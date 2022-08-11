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

#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/precondition.h"
#include "picolibrary/wiznet/w5500.h"

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

/**
 * \brief TCP/UDP over IP port allocator without ephemeral port allocation support.
 */
class TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation {
  public:
    /**
     * \brief Constructor.
     */
    constexpr TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] socket_protocol The socket protocol the port allocator will support.
     *
     * \pre socket_protocol == picolibrary::WIZnet::W5500::Socket_Protocol::TCP or
     *      socket_protocol == picolibrary::WIZnet::W5500::Socket_Protocol::UDP
     */
    constexpr TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation( Socket_Protocol socket_protocol ) noexcept
        :
        m_socket_protocol{ socket_protocol }
    {
        expect(
            socket_protocol == Socket_Protocol::TCP or socket_protocol == Socket_Protocol::UDP,
            Generic_Error::INVALID_ARGUMENT );
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation(
        TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation(
        TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation && expression ) noexcept
        -> TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation const & expression ) noexcept
        -> TCP_UDP_Port_Allocator_Without_Ephemeral_Port_Allocation & = default;

    /**
     * \brief Allocate a port.
     *
     * \tparam Driver The type of driver used to interact with the W5500.
     *
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] port The port to allocate.
     *
     * \pre not port.is_any()
     * \pre port is not already in use
     *
     * \return port
     */
    template<typename Driver>
    auto allocate( Driver const & driver, ::picolibrary::IP::Port port ) noexcept
        -> ::picolibrary::IP::Port
    {
        expect( not port.is_any(), Generic_Error::LOGIC_ERROR );
        expect( not port_is_in_use( driver, port ), Generic_Error::LOGIC_ERROR );

        return port;
    }

    /**
     * \brief Deallocate a previously allocated port.
     *
     * \param[in] port The previously allocated port to deallocate.
     */
    constexpr void deallocate( ::picolibrary::IP::Port port ) noexcept
    {
        static_cast<void>( port );
    }

  private:
    /**
     * \brief The socket protocol the port allocator supports.
     */
    Socket_Protocol m_socket_protocol{};

    /**
     * \brief Check if a port is in use.
     *
     * \tparam Driver The type of driver used to interact with the W5500.
     *
     * \param[in] driver The driver used to interact with the W5500.
     * \param[in] port The port to check.
     *
     * \return true if port is in use.
     * \return false if port is not in use.
     */
    template<typename Driver>
    // NOLINTNEXTLINE(readability-function-size)
    auto port_is_in_use( Driver const & driver, ::picolibrary::IP::Port port ) noexcept -> bool
    {
        // #lizard forgives the length

        Socket_ID const socket_ids[]{
            // clang-format off

            Socket_ID::_0,
            Socket_ID::_1,
            Socket_ID::_2,
            Socket_ID::_3,
            Socket_ID::_4,
            Socket_ID::_5,
            Socket_ID::_6,
            Socket_ID::_7,

            // clang-format on
        };

        for ( auto const socket_id : socket_ids ) {
            if ( static_cast<Socket_Protocol>( driver.read_sn_mr( socket_id ) & SN_MR::Mask::P ) == m_socket_protocol
                 and driver.read_sn_port( socket_id ) == port ) {
                return true;
            } // if
        }     // for

        return false;
    }
};

} // namespace picolibrary::WIZnet::W5500::IP

#endif // PICOLIBRARY_WIZNET_W5500_IP_H
