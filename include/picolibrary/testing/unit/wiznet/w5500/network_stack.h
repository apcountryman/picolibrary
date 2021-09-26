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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500::Mock_Network_Stack interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_NETWORK_STACK_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_NETWORK_STACK_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Unit::WIZnet::W5500 {

/**
 * \brief Mock WIZnet W5500 IP network stack.
 */
class Mock_Network_Stack {
  public:
    /**
     * \brief Constructor.
     */
    Mock_Network_Stack() = default;

    Mock_Network_Stack( Mock_Network_Stack && ) = delete;

    Mock_Network_Stack( Mock_Network_Stack const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Network_Stack() noexcept = default;

    auto operator=( Mock_Network_Stack && ) = delete;

    auto operator=( Mock_Network_Stack const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), ping_w5500, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_phy, ( ::picolibrary::WIZnet::W5500::PHY_Mode ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::PHY_Mode, Error_Code>), phy_mode, (), ( const ) );

    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::Link_Status, Error_Code>), link_status, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::Link_Mode, Error_Code>), link_mode, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::Link_Speed, Error_Code>), link_speed, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_ping_blocking, ( ::picolibrary::WIZnet::W5500::Ping_Blocking ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::Ping_Blocking, Error_Code>), ping_blocking_configuration, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_arp_forcing, ( ::picolibrary::WIZnet::W5500::ARP_Forcing ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::ARP_Forcing, Error_Code>), arp_forcing_configuration, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_retransmission, ( std::uint16_t, std::uint8_t ) );
    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), retry_time, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), retry_count, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_socket_buffers, ( ::picolibrary::WIZnet::W5500::Buffer_Size ) );
    MOCK_METHOD( (Result<::picolibrary::WIZnet::W5500::Buffer_Size, Error_Code>), socket_buffer_size, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_mac_address, (MAC_Address const &));
    MOCK_METHOD( (Result<MAC_Address, Error_Code>), mac_address, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_ip_address, (IPv4::Address const &));
    MOCK_METHOD( (Result<IPv4::Address, Error_Code>), ip_address, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_gateway_ip_address, (IPv4::Address const &));
    MOCK_METHOD( (Result<IPv4::Address, Error_Code>), gateway_ip_address, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_subnet_mask, (IPv4::Address const &));
    MOCK_METHOD( (Result<IPv4::Address, Error_Code>), subnet_mask, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_interrupt_assert_wait_time, ( std::uint16_t ) );
    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), interrupt_assert_wait_time, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, () );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), enabled_interrupts, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), interrupt_context, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), enabled_socket_interrupts, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), socket_interrupt_context, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), service, () );
};

} // namespace picolibrary::Testing::Unit::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_NETWORK_STACK_H