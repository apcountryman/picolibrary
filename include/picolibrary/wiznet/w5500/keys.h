/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::WIZnet::W5500 keys interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_KEYS_H
#define PICOLIBRARY_WIZNET_W5500_IP_KEYS_H

namespace picolibrary::WIZnet::W5500::IP {

template<typename Controller, typename Device_Selector, typename TCP_Port_Allocator, typename UDP_Port_Allocator, typename Driver>
class Network_Stack;

} // namespace picolibrary::WIZnet::W5500::IP

namespace picolibrary::WIZnet::W5500::IP::TCP {

template<typename Network_Stack>
class Client;

template<typename Network_Stack, typename Server>
class Server_Connection_Handler;

template<typename Network_Stack>
class Server;

} // namespace picolibrary::WIZnet::W5500::IP::TCP

namespace picolibrary::WIZnet::W5500::IP::UDP {

template<typename Network_Stack>
class Socket;

} // namespace picolibrary::WIZnet::W5500::IP::UDP

namespace picolibrary::WIZnet::W5500 {

/**
 * \brief picolibrary::WIZnet::W5500 socket construction key.
 */
class Socket_Construction_Key {
  public:
    Socket_Construction_Key( Socket_Construction_Key && ) = delete;

    Socket_Construction_Key( Socket_Construction_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Socket_Construction_Key() noexcept = default;

    auto operator=( Socket_Construction_Key && ) = delete;

    auto operator=( Socket_Construction_Key const & ) = delete;

  private:
    template<typename Controller, typename Device_Selector, typename TCP_Port_Allocator, typename UDP_Port_Allocator, typename Driver>
    friend class Network_Stack;

    /**
     * \brief Constructor.
     */
    constexpr Socket_Construction_Key() noexcept = default;
};

} // namespace picolibrary::WIZnet::W5500

namespace picolibrary::WIZnet::W5500::IP {

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack driver access key.
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
    template<typename Network_Stack>
    friend class TCP::Client;

    template<typename Network_Stack, typename Server>
    friend class TCP::Server_Connection_Handler;

    template<typename Network_Stack>
    friend class TCP::Server;

    template<typename Network_Stack>
    friend class UDP::Socket;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_Driver_Access_Key() noexcept = default;
};

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack socket allocation key.
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
    template<typename Network_Stack>
    friend class TCP::Client;

    template<typename Network_Stack, typename Server>
    friend class TCP::Server_Connection_Handler;

    template<typename Network_Stack>
    friend class TCP::Server;

    template<typename Network_Stack>
    friend class UDP::Socket;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_Socket_Allocation_Key() noexcept = default;
};

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack
 *        picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler detachment key.
 */
class Network_Stack_TCP_Server_Connection_Handler_Detachment_Key {
  public:
    Network_Stack_TCP_Server_Connection_Handler_Detachment_Key(
        Network_Stack_TCP_Server_Connection_Handler_Detachment_Key && ) = delete;

    Network_Stack_TCP_Server_Connection_Handler_Detachment_Key(
        Network_Stack_TCP_Server_Connection_Handler_Detachment_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Network_Stack_TCP_Server_Connection_Handler_Detachment_Key() noexcept = default;

    auto operator=( Network_Stack_TCP_Server_Connection_Handler_Detachment_Key && ) = delete;

    auto operator=( Network_Stack_TCP_Server_Connection_Handler_Detachment_Key const & ) = delete;

  private:
    template<typename Network_Stack>
    friend class TCP::Server;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_TCP_Server_Connection_Handler_Detachment_Key() noexcept = default;
};

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack TCP port allocator access key.
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
    template<typename Network_Stack>
    friend class TCP::Client;

    template<typename Network_Stack, typename Server>
    friend class TCP::Server_Connection_Handler;

    template<typename Network_Stack>
    friend class TCP::Server;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_TCP_Port_Allocator_Access_Key() noexcept = default;
};

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack UDP port allocator access key.
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
    template<typename Network_Stack>
    friend class UDP::Socket;

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack_UDP_Port_Allocator_Access_Key() noexcept = default;
};

} // namespace picolibrary::WIZnet::W5500::IP

namespace picolibrary::WIZnet::W5500::IP::TCP {

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server socket deallocation key.
 */
class Server_Socket_Deallocation_Key {
  public:
    Server_Socket_Deallocation_Key( Server_Socket_Deallocation_Key && ) = delete;

    Server_Socket_Deallocation_Key( Server_Socket_Deallocation_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Server_Socket_Deallocation_Key() noexcept = default;

    auto operator=( Server_Socket_Deallocation_Key && ) = delete;

    auto operator=( Server_Socket_Deallocation_Key const & ) = delete;

  private:
    template<typename Network_Stack, typename Server>
    friend class Server_Connection_Handler;

    /**
     * \brief Constructor.
     */
    constexpr Server_Socket_Deallocation_Key() noexcept = default;
};

/**
 * \brief picolibrary::WIZnet::W5500::IP::TCP::Server_Connection_Handler construction key.
 */
class Server_Connection_Handler_Construction_Key {
  public:
    Server_Connection_Handler_Construction_Key( Server_Connection_Handler_Construction_Key && ) = delete;

    Server_Connection_Handler_Construction_Key( Server_Connection_Handler_Construction_Key const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Server_Connection_Handler_Construction_Key() noexcept = default;

    auto operator=( Server_Connection_Handler_Construction_Key && ) = delete;

    auto operator=( Server_Connection_Handler_Construction_Key const & ) = delete;

  private:
    template<typename Network_Stack>
    friend class TCP::Server;

    /**
     * \brief Constructor.
     */
    constexpr Server_Connection_Handler_Construction_Key() noexcept = default;
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_KEYS_H
