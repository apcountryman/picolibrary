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
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack keys interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_KEYS_H
#define PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_KEYS_H

namespace picolibrary::WIZnet::W5500::IP {

template<typename Controller, typename Device_Selector, typename TCP_Port_Allocator, typename UDP_Port_Allocator, typename Driver>
class Network_Stack;

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack driver access key.
 */
class Network_Stack_Driver_Access_Key {
  public:
    Network_Stack_Driver_Access_Key( Network_Stack_Driver_Access_Key && ) = delete;

    Network_Stack_Driver_Access_Key( Network_Stack_Driver_Access_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_Driver_Access_Key() noexcept = default;

    auto operator=( Network_Stack_Driver_Access_Key && ) = delete;

    auto operator=( Network_Stack_Driver_Access_Key const & ) = delete;

  private:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_Driver_Access_Key() noexcept = default;
};

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack socket allocation key.
 */
class Network_Stack_Socket_Allocation_Key {
  public:
    Network_Stack_Socket_Allocation_Key( Network_Stack_Socket_Allocation_Key && ) = delete;

    Network_Stack_Socket_Allocation_Key( Network_Stack_Socket_Allocation_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_Socket_Allocation_Key() noexcept = default;

    auto operator=( Network_Stack_Socket_Allocation_Key && ) = delete;

    auto operator=( Network_Stack_Socket_Allocation_Key const & ) = delete;

  private:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_Socket_Allocation_Key() noexcept = default;
};

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack socket construction key.
 */
class Network_Stack_Socket_Construction_Key {
  public:
    Network_Stack_Socket_Construction_Key( Network_Stack_Socket_Construction_Key && ) = delete;

    Network_Stack_Socket_Construction_Key( Network_Stack_Socket_Construction_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_Socket_Construction_Key() noexcept = default;

    auto operator=( Network_Stack_Socket_Construction_Key && ) = delete;

    auto operator=( Network_Stack_Socket_Construction_Key const & ) = delete;

  private:
    template<typename Controller, typename Device_Selector, typename TCP_Port_Allocator, typename UDP_Port_Allocator, typename Driver>
    friend class Network_Stack;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_Socket_Construction_Key() noexcept = default;
};

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack TCP server socket detachment
 *        key.
 */
class Network_Stack_TCP_Server_Detachment_Key {
  public:
    Network_Stack_TCP_Server_Detachment_Key( Network_Stack_TCP_Server_Detachment_Key && ) = delete;

    Network_Stack_TCP_Server_Detachment_Key( Network_Stack_TCP_Server_Detachment_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_TCP_Server_Detachment_Key() noexcept = default;

    auto operator=( Network_Stack_TCP_Server_Detachment_Key && ) = delete;

    auto operator=( Network_Stack_TCP_Server_Detachment_Key const & ) = delete;

  private:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_TCP_Server_Detachment_Key() noexcept = default;
};

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack TCP port allocator access key.
 */
class Network_Stack_TCP_Port_Allocator_Access_Key {
  public:
    Network_Stack_TCP_Port_Allocator_Access_Key( Network_Stack_TCP_Port_Allocator_Access_Key && ) = delete;

    Network_Stack_TCP_Port_Allocator_Access_Key( Network_Stack_TCP_Port_Allocator_Access_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_TCP_Port_Allocator_Access_Key() noexcept = default;

    auto operator=( Network_Stack_TCP_Port_Allocator_Access_Key && ) = delete;

    auto operator=( Network_Stack_TCP_Port_Allocator_Access_Key const & ) = delete;

  private:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_TCP_Port_Allocator_Access_Key() noexcept = default;
};

/**
 * \brief ::picolibrary::WIZnet::W5500::IP::Network_Stack UDP port allocator access key.
 */
class Network_Stack_UDP_Port_Allocator_Access_Key {
  public:
    Network_Stack_UDP_Port_Allocator_Access_Key( Network_Stack_UDP_Port_Allocator_Access_Key && ) = delete;

    Network_Stack_UDP_Port_Allocator_Access_Key( Network_Stack_UDP_Port_Allocator_Access_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_UDP_Port_Allocator_Access_Key() noexcept = default;

    auto operator=( Network_Stack_UDP_Port_Allocator_Access_Key && ) = delete;

    auto operator=( Network_Stack_UDP_Port_Allocator_Access_Key const & ) = delete;

  private:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_UDP_Port_Allocator_Access_Key() noexcept = default;
};

} // namespace picolibrary::WIZnet::W5500::IP

#endif // PICOLIBRARY_WIZNET_W5500_IP_NETWORK_STACK_KEYS_H
