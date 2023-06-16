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
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack automated test program.
 */

#include <array>
#include <bitset>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/array.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/testing/automated/wiznet/w5500/ip.h"
#include "picolibrary/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500/ip/network_stack.h"

namespace {

using ::picolibrary::Array;
using ::picolibrary::MAC_Address;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::WIZnet::W5500::Mock_Driver;
using ::picolibrary::Testing::Automated::WIZnet::W5500::IP::Mock_Port_Allocator;
using ::picolibrary::WIZnet::W5500::ARP_Forcing;
using ::picolibrary::WIZnet::W5500::Link_Mode;
using ::picolibrary::WIZnet::W5500::Link_Speed;
using ::picolibrary::WIZnet::W5500::Link_Status;
using ::picolibrary::WIZnet::W5500::PHY_Mode;
using ::picolibrary::WIZnet::W5500::Ping_Blocking;
using ::picolibrary::WIZnet::W5500::Socket_Buffer_Size;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::IP::Network_Stack;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using IPv4_Address = ::picolibrary::IPv4::Address;

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::Network_Stack( Driver &,
 *        picolibrary::Error_Code const &, TCP_Port_Allocator ) works properly.
 */
TEST( constructor, worksProperly )
{
    auto       driver                     = Mock_Driver{};
    auto       tcp_port_allocator         = Mock_Port_Allocator{};
    auto const nonresponsive_device_error = Mock_Error{ 58 };

    auto const network_stack = Network_Stack{ driver,
                                              nonresponsive_device_error,
                                              tcp_port_allocator.handle() };

    EXPECT_EQ( network_stack.nonresponsive_device_error(), nonresponsive_device_error );
    EXPECT_EQ( network_stack.socket_buffer_size(), Socket_Buffer_Size::_0_KiB );
    EXPECT_EQ( network_stack.sockets(), 0 );
    EXPECT_EQ( network_stack.sockets_available_for_allocation(), 0 );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive() test case.
 */
struct w5500IsResponsive_Test_Case {
    /**
     * \brief The VERSIONR register value.
     */
    std::uint8_t versionr;

    /**
     * \brief The W5500 is responsive.
     */
    bool w5500_is_responsive;
};

auto operator<<( std::ostream & stream, w5500IsResponsive_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".versionr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.versionr )
                  << ", "
                  << ".w5500_is_responsive = " << std::boolalpha << test_case.w5500_is_responsive
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive() test
 *        fixture.
 */
class w5500IsResponsive : public TestWithParam<w5500IsResponsive_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive()
 *        works properly.
 */
TEST_P( w5500IsResponsive, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 243 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_versionr() ).WillOnce( Return( test_case.versionr ) );

    ASSERT_EQ( network_stack.w5500_is_responsive(), test_case.w5500_is_responsive );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::w5500_is_responsive() test cases.
 */
w5500IsResponsive_Test_Case const w5500IsResponsive_TEST_CASES[]{
    // clang-format off

    { 0x00, false },
    { 0x01, false },
    { 0x03, false },
    { 0x04, true  },
    { 0x05, false },
    { 0x2F, false },
    { 0xFF, false },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, w5500IsResponsive, ValuesIn( w5500IsResponsive_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy() test case.
 */
struct configurePHY_Test_Case {
    /**
     * \brief The desired PHY mode.
     */
    PHY_Mode phy_mode;

    /**
     * \brief The PHYCFGR register value.
     */
    std::uint8_t phycfgr;
};

auto operator<<( std::ostream & stream, configurePHY_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".phy_mode = " << test_case.phy_mode
                  << ", "
                  << ".phycfgr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.phycfgr }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy() test fixture.
 */
class configurePHY : public TestWithParam<configurePHY_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy() works
 *        properly.
 */
TEST_P( configurePHY, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 39 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr | 0b1'0'000'0'0'0 ) );
    EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr ) );
    EXPECT_CALL( driver, write_phycfgr( test_case.phycfgr | 0b1'0'000'0'0'0 ) );

    network_stack.configure_phy( test_case.phy_mode );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_phy() test cases.
 */
configurePHY_Test_Case const configurePHY_TEST_CASES[]{
    // clang-format off

    { PHY_Mode::CONFIGURED_BY_HARDWARE,                       0b0'0'000'0'0'0 },
    { PHY_Mode::POWER_DOWN,                                   0b0'1'110'0'0'0 },
    { PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED,  0b0'1'000'0'0'0 },
    { PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED,  0b0'1'001'0'0'0 },
    { PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED, 0b0'1'010'0'0'0 },
    { PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED, 0b0'1'011'0'0'0 },
    { PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED,  0b0'1'100'0'0'0 },
    { PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED,         0b0'1'111'0'0'0 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configurePHY, ValuesIn( configurePHY_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode() test case.
 */
struct phyMode_Test_Case {
    /**
     * \brief The PHYCFGR register value.
     */
    std::uint8_t phycfgr;

    /**
     * \brief The PHY mode.
     */
    PHY_Mode phy_mode;
};

auto operator<<( std::ostream & stream, phyMode_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".phycfgr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.phycfgr }
                  << ", "
                  << ".phy_mode = " << test_case.phy_mode
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode() test fixture.
 */
class phyMode : public TestWithParam<phyMode_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode() works properly.
 */
TEST_P( phyMode, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver, Mock_Error{ 85 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( test_case.phycfgr ) );

    ASSERT_EQ( network_stack.phy_mode(), test_case.phy_mode );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::phy_mode() test cases.
 */
phyMode_Test_Case const phyMode_TEST_CASES[]{
    // clang-format off

    { 0b0'0'000'1'0'1, PHY_Mode::CONFIGURED_BY_HARDWARE                       },
    { 0b0'1'110'1'0'1, PHY_Mode::POWER_DOWN                                   },
    { 0b0'1'000'1'0'1, PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED  },
    { 0b0'1'001'1'0'1, PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED  },
    { 0b0'1'010'1'0'1, PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED },
    { 0b0'1'011'1'0'1, PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED },
    { 0b0'1'100'1'0'1, PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED  },
    { 0b0'1'111'1'0'1, PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED         },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, phyMode, ValuesIn( phyMode_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_status() test case.
 */
struct linkStatus_Test_Case {
    /**
     * \brief The PHYCFGR register value.
     */
    std::uint8_t phycfgr;

    /**
     * \brief The link status.
     */
    Link_Status link_status;
};

auto operator<<( std::ostream & stream, linkStatus_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".phycfgr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.phycfgr }
                  << ", "
                  << ".link_status = " << test_case.link_status
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_status() test fixture.
 */
class linkStatus : public TestWithParam<linkStatus_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_status() works
 *        properly.
 */
TEST_P( linkStatus, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver, Mock_Error{ 16 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( test_case.phycfgr ) );

    ASSERT_EQ( network_stack.link_status(), test_case.link_status );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_status() test cases.
 */
linkStatus_Test_Case const linkStatus_TEST_CASES[]{
    // clang-format off

    { 0b1'0'010'0'1'0, Link_Status::DOWN },
    { 0b1'0'010'0'1'1, Link_Status::UP   },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, linkStatus, ValuesIn( linkStatus_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode() test case.
 */
struct linkMode_Test_Case {
    /**
     * \brief The PHYCFGR register value.
     */
    std::uint8_t phycfgr;

    /**
     * \brief The link mode.
     */
    Link_Mode link_mode;
};

auto operator<<( std::ostream & stream, linkMode_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".phycfgr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.phycfgr }
                  << ", "
                  << ".link_mode = " << test_case.link_mode
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode() test fixture.
 */
class linkMode : public TestWithParam<linkMode_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode() works
 *        properly.
 */
TEST_P( linkMode, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 106 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( test_case.phycfgr ) );

    ASSERT_EQ( network_stack.link_mode(), test_case.link_mode );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_mode() test cases.
 */
linkMode_Test_Case const linkMode_TEST_CASES[]{
    // clang-format off

    { 0b0'1'011'0'0'1, Link_Mode::HALF_DUPLEX },
    { 0b0'1'011'1'0'1, Link_Mode::FULL_DUPLEX },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, linkMode, ValuesIn( linkMode_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed() test case.
 */
struct linkSpeed_Test_Case {
    /**
     * \brief The PHYCFGR register value.
     */
    std::uint8_t phycfgr;

    /**
     * \brief The link speed.
     */
    Link_Speed link_speed;
};

auto operator<<( std::ostream & stream, linkSpeed_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".phycfgr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.phycfgr }
                  << ", "
                  << ".link_speed = " << test_case.link_speed
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed() test fixture.
 */
class linkSpeed : public TestWithParam<linkSpeed_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed() works
 *        properly.
 */
TEST_P( linkSpeed, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 125 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_phycfgr() ).WillOnce( Return( test_case.phycfgr ) );

    ASSERT_EQ( network_stack.link_speed(), test_case.link_speed );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::link_speed() test cases.
 */
linkSpeed_Test_Case const linkSpeed_TEST_CASES[]{
    // clang-format off

    { 0b0'1'111'0'0'0, Link_Speed::_10_MbPs  },
    { 0b0'1'111'0'1'0, Link_Speed::_100_MbPs },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, linkSpeed, ValuesIn( linkSpeed_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking() test
 *        case.
 */
struct configurePingBlocking_Test_Case {
    /**
     * \brief The desired ping blocking configuration.
     */
    Ping_Blocking ping_blocking_configuration;

    /**
     * \brief The MR register value.
     */
    std::uint8_t mr;
};

auto operator<<( std::ostream & stream, configurePingBlocking_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".ping_blocking_configuration = " << test_case.ping_blocking_configuration
                  << ", "
                  << ".mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.mr }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking() test
 *        fixture.
 */
class configurePingBlocking : public TestWithParam<configurePingBlocking_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking()
 *        works properly.
 */
TEST_P( configurePingBlocking, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 83 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( 0b1'1'0'1'0'0'1'1 ) );
    EXPECT_CALL( driver, write_mr( test_case.mr ) );

    network_stack.configure_ping_blocking( test_case.ping_blocking_configuration );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ping_blocking() test
 *        cases.
 */
configurePingBlocking_Test_Case const configurePingBlocking_TEST_CASES[]{
    // clang-format off

    { Ping_Blocking::DISABLED, 0b1'1'0'0'0'0'1'1 },
    { Ping_Blocking::ENABLED,  0b1'1'0'1'0'0'1'1 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configurePingBlocking, ValuesIn( configurePingBlocking_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()
 *        test case.
 */
struct pingBlockingConfiguration_Test_Case {
    /**
     * \brief The MR register value.
     */
    std::uint8_t mr;

    /**
     * \brief The ping blocking configuration.
     */
    Ping_Blocking ping_blocking_configuration;
};

auto operator<<( std::ostream & stream, pingBlockingConfiguration_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.mr }
                  << ", "
                  << ".ping_blocking_configuration = " << test_case.ping_blocking_configuration
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()
 *        test fixture.
 */
class pingBlockingConfiguration : public TestWithParam<pingBlockingConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()
 *        works properly.
 */
TEST_P( pingBlockingConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 132 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( test_case.mr ) );

    ASSERT_EQ( network_stack.ping_blocking_configuration(), test_case.ping_blocking_configuration );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::ping_blocking_configuration()
 *        test cases.
 */
pingBlockingConfiguration_Test_Case const pingBlockingConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b1'1'0'0'0'0'1'1, Ping_Blocking::DISABLED },
    { 0b1'1'0'1'0'0'1'1, Ping_Blocking::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pingBlockingConfiguration, ValuesIn( pingBlockingConfiguration_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing() test
 *        case.
 */
struct configureARPForcing_Test_Case {
    /**
     * \brief The desired ARP forcing configuration.
     */
    ARP_Forcing arp_forcing_configuration;

    /**
     * \brief The MR register value.
     */
    std::uint8_t mr;
};

auto operator<<( std::ostream & stream, configureARPForcing_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".arp_forcing_configuration = " << test_case.arp_forcing_configuration
                  << ", "
                  << ".mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.mr }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing() test
 *        fixture.
 */
class configureARPForcing : public TestWithParam<configureARPForcing_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing()
 *        works properly.
 */
TEST_P( configureARPForcing, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 103 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( 0b1'0'0'1'0'0'0'0 ) );
    EXPECT_CALL( driver, write_mr( test_case.mr ) );

    network_stack.configure_arp_forcing( test_case.arp_forcing_configuration );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_arp_forcing() test
 *        cases.
 */
configureARPForcing_Test_Case const configureARPForcing_TEST_CASES[]{
    // clang-format off

    { ARP_Forcing::DISABLED, 0b1'0'0'1'0'0'0'0 },
    { ARP_Forcing::ENABLED,  0b1'0'0'1'0'0'1'0 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configureARPForcing, ValuesIn( configureARPForcing_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration() test
 *        case.
 */
struct arpForcingConfiguration_Test_Case {
    /**
     * \brief The MR register value.
     */
    std::uint8_t mr;

    /**
     * \brief The ARP forcing configuration.
     */
    ARP_Forcing arp_forcing_configuration;
};

auto operator<<( std::ostream & stream, arpForcingConfiguration_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".mr = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.mr }
                  << ", "
                  << ".arp_forcing_configuration = " << test_case.arp_forcing_configuration
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration() test
 *        fixture.
 */
class arpForcingConfiguration : public TestWithParam<arpForcingConfiguration_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration() works
 *        properly.
 */
TEST_P( arpForcingConfiguration, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 168 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_mr() ).WillOnce( Return( test_case.mr ) );

    ASSERT_EQ( network_stack.arp_forcing_configuration(), test_case.arp_forcing_configuration );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::arp_forcing_configuration() test
 *        cases.
 */
arpForcingConfiguration_Test_Case const arpForcingConfiguration_TEST_CASES[]{
    // clang-format off

    { 0b0'1'0'0'0'1'0'0, ARP_Forcing::DISABLED },
    { 0b0'1'0'0'0'1'1'0, ARP_Forcing::ENABLED  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, arpForcingConfiguration, ValuesIn( arpForcingConfiguration_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_retransmission()
 *        works properly.
 */
TEST( configureRetransmission, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 35 }, tcp_port_allocator.handle() };

    auto const retry_time  = std::uint16_t{ 0xB2FA };
    auto const retry_count = std::uint8_t{ 0xC2 };

    EXPECT_CALL( driver, write_rtr( retry_time ) );
    EXPECT_CALL( driver, write_rcr( retry_count ) );

    network_stack.configure_retransmission( retry_time, retry_count );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::retry_time() works
 *        properly.
 */
TEST( retryTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 245 },
                                              tcp_port_allocator.handle() };

    auto const rtr = std::uint16_t{ 0x8BBE };

    EXPECT_CALL( driver, read_rtr() ).WillOnce( Return( rtr ) );

    ASSERT_EQ( network_stack.retry_time(), rtr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::retry_count() works
 *        properly.
 */
TEST( retryCount, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 182 },
                                              tcp_port_allocator.handle() };

    auto const rcr = std::uint8_t{ 0xEC };

    EXPECT_CALL( driver, read_rcr() ).WillOnce( Return( rcr ) );

    ASSERT_EQ( network_stack.retry_count(), rcr );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()
 *        test case.
 */
struct configureSocketBufferSize_Test_Case {
    /**
     * \brief The desired socket buffer size.
     */
    Socket_Buffer_Size socket_buffer_size;

    /**
     * \brief The SN_RXBUF_SIZE and SN_TXBUF_SIZE register values.
     */
    std::array<std::uint8_t, 8> sn_xxbuf_size;

    /**
     * \brief The number of sockets the network stack is configured to support.
     */
    std::uint_fast8_t sockets;
};

auto operator<<( std::ostream & stream, configureSocketBufferSize_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_buffer_size = " << test_case.socket_buffer_size
                  << ", "
                  << ".sn_xxbuf_size = { " << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 0 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 1 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 2 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 3 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 4 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 5 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 6 ] ) << ", "
                                           << static_cast<std::uint_fast16_t>( test_case.sn_xxbuf_size[ 7 ] ) << " }"
                  << ", "
                  << ".sockets = " << static_cast<std::uint16_t>( test_case.sockets )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()
 *        test fixture.
 */
class configureSocketBufferSize : public TestWithParam<configureSocketBufferSize_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()
 *        works properly.
 */
TEST_P( configureSocketBufferSize, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 196 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_0, test_case.sn_xxbuf_size[ 0 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_0, test_case.sn_xxbuf_size[ 0 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_1, test_case.sn_xxbuf_size[ 1 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_1, test_case.sn_xxbuf_size[ 1 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_2, test_case.sn_xxbuf_size[ 2 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_2, test_case.sn_xxbuf_size[ 2 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_3, test_case.sn_xxbuf_size[ 3 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_3, test_case.sn_xxbuf_size[ 3 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_4, test_case.sn_xxbuf_size[ 4 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_4, test_case.sn_xxbuf_size[ 4 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_5, test_case.sn_xxbuf_size[ 5 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_5, test_case.sn_xxbuf_size[ 5 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_6, test_case.sn_xxbuf_size[ 6 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_6, test_case.sn_xxbuf_size[ 6 ] ) );
    EXPECT_CALL( driver, write_sn_rxbuf_size( Socket_ID::_7, test_case.sn_xxbuf_size[ 7 ] ) );
    EXPECT_CALL( driver, write_sn_txbuf_size( Socket_ID::_7, test_case.sn_xxbuf_size[ 7 ] ) );

    network_stack.configure_socket_buffer_size( test_case.socket_buffer_size );

    EXPECT_EQ( network_stack.socket_buffer_size(), test_case.socket_buffer_size );
    EXPECT_EQ( network_stack.sockets(), test_case.sockets );
    EXPECT_EQ( network_stack.sockets_available_for_allocation(), test_case.sockets );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::configure_socket_buffer_size()
 *        test cases.
 */
configureSocketBufferSize_Test_Case const configureSocketBufferSize_TEST_CASES[]{
    // clang-format off

    { Socket_Buffer_Size::_2_KiB,  {  2, 2, 2, 2, 2, 2, 2, 2 }, 8 },
    { Socket_Buffer_Size::_4_KiB,  {  4, 4, 4, 4, 0, 0, 0, 0 }, 4 },
    { Socket_Buffer_Size::_8_KiB,  {  8, 8, 0, 0, 0, 0, 0, 0 }, 2 },
    { Socket_Buffer_Size::_16_KiB, { 16, 0, 0, 0, 0, 0, 0, 0 }, 1 },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, configureSocketBufferSize, ValuesIn( configureSocketBufferSize_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_mac_address()
 *        works properly.
 */
TEST( configureMACAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 177 }, tcp_port_allocator.handle() };

    auto const address = MAC_Address{ { 0xB4, 0x49, 0x7C, 0xBB, 0xF9, 0x8C } };

    EXPECT_CALL( driver, write_shar( address.as_byte_array() ) );

    network_stack.configure_mac_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::mac_address() works
 *        properly.
 */
TEST( macAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 186 },
                                              tcp_port_allocator.handle() };

    auto const shar = Array<std::uint8_t, 6>{ 0x43, 0xAD, 0x77, 0x0A, 0xC7, 0xFE };

    EXPECT_CALL( driver, read_shar() ).WillOnce( Return( shar ) );

    ASSERT_EQ( network_stack.mac_address().as_byte_array(), shar );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_address()
 *        works properly.
 */
TEST( configureIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 203 }, tcp_port_allocator.handle() };

    auto const address = IPv4_Address{ { 22, 84, 250, 184 } };

    EXPECT_CALL( driver, write_sipr( address.as_byte_array() ) );

    network_stack.configure_ipv4_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_address() works
 *        properly.
 */
TEST( ipv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 120 },
                                              tcp_port_allocator.handle() };

    auto const sipr = Array<std::uint8_t, 4>{ 242, 69, 197, 219 };

    EXPECT_CALL( driver, read_sipr() ).WillOnce( Return( sipr ) );

    ASSERT_EQ( network_stack.ipv4_address().as_byte_array(), sipr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_gateway_address()
 *        works properly.
 */
TEST( configureIPv4GatewayAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 222 }, tcp_port_allocator.handle() };

    auto const address = IPv4_Address{ { 195, 53, 124, 168 } };

    EXPECT_CALL( driver, write_gar( address.as_byte_array() ) );

    network_stack.configure_ipv4_gateway_address( address );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_gateway_address()
 *        works properly.
 */
TEST( ipv4GatewayAddress, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver, Mock_Error{ 91 }, tcp_port_allocator.handle() };

    auto const gar = Array<std::uint8_t, 4>{ 163, 8, 45, 250 };

    EXPECT_CALL( driver, read_gar() ).WillOnce( Return( gar ) );

    ASSERT_EQ( network_stack.ipv4_gateway_address().as_byte_array(), gar );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_ipv4_subnet_mask()
 *        works properly.
 */
TEST( configureIPv4SubnetMask, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 184 }, tcp_port_allocator.handle() };

    auto const subnet_mask = IPv4_Address{ { 119, 122, 231, 41 } };

    EXPECT_CALL( driver, write_subr( subnet_mask.as_byte_array() ) );

    network_stack.configure_ipv4_subnet_mask( subnet_mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::ipv4_subnet_mask() works
 *        properly.
 */
TEST( ipv4SubnetMask, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 169 },
                                              tcp_port_allocator.handle() };

    auto const subr = Array<std::uint8_t, 4>{ 94, 66, 94, 244 };

    EXPECT_CALL( driver, read_subr() ).WillOnce( Return( subr ) );

    ASSERT_EQ( network_stack.ipv4_subnet_mask().as_byte_array(), subr );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::configure_interrupt_assert_wait_time()
 *        works properly.
 */
TEST( configureInterruptAssertWaitTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 114 }, tcp_port_allocator.handle() };

    auto const interrupt_assert_wait_time = std::uint16_t{ 0xB752 };

    EXPECT_CALL( driver, write_intlevel( interrupt_assert_wait_time ) );

    network_stack.configure_interrupt_assert_wait_time( interrupt_assert_wait_time );
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_assert_wait_time()
 *        works properly.
 */
TEST( interruptAssertWaitTime, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 238 },
                                              tcp_port_allocator.handle() };

    auto const intlevel = std::uint16_t{ 0x8724 };

    EXPECT_CALL( driver, read_intlevel() ).WillOnce( Return( intlevel ) );

    ASSERT_EQ( network_stack.interrupt_assert_wait_time(), intlevel );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enable_interrupts() works
 *        properly.
 */
TEST( enableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 26 }, tcp_port_allocator.handle() };

    auto const mask = std::uint8_t{ 0b0101'0100 };

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( 0b0011'0101 ) );
    EXPECT_CALL( driver, write_imr( 0b0111'0101 ) );

    network_stack.enable_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts(
 *        std::uint8_t ) works properly.
 */
TEST( disableInterrupts, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 204 }, tcp_port_allocator.handle() };

    auto const mask = std::uint8_t{ 0b0101'0001 };

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( 0b0011'1100 ) );
    EXPECT_CALL( driver, write_imr( 0b0010'1100 ) );

    network_stack.disable_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::disable_interrupts() works
 *        properly.
 */
TEST( disableAllInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 148 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_imr( 0x00 ) );

    network_stack.disable_interrupts();
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enabled_interrupts() works
 *        properly.
 */
TEST( enabledInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 190 },
                                              tcp_port_allocator.handle() };

    auto const imr = std::uint8_t{ 0b00010100 };

    EXPECT_CALL( driver, read_imr() ).WillOnce( Return( imr ) );

    ASSERT_EQ( network_stack.enabled_interrupts(), imr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::interrupt_context() works
 *        properly.
 */
TEST( interruptContext, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver, Mock_Error{ 82 }, tcp_port_allocator.handle() };

    auto const ir = std::uint8_t{ 0b00010101 };

    EXPECT_CALL( driver, read_ir() ).WillOnce( Return( ir ) );

    ASSERT_EQ( network_stack.interrupt_context(), ir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::clear_interrupts() works
 *        properly.
 */
TEST( clearInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 251 }, tcp_port_allocator.handle() };

    auto const mask = std::uint8_t{ 0b00110111 };

    EXPECT_CALL( driver, write_ir( mask ) );

    network_stack.clear_interrupts( mask );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::enable_socket_interrupts()
 *        works properly.
 */
TEST( enableSocketInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 191 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_simr( 0xFF ) );

    network_stack.enable_socket_interrupts();
}

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::disable_socket_interrupts() works
 *        properly.
 */
TEST( disableSocketInterrupts, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto network_stack = Network_Stack{ driver, Mock_Error{ 32 }, tcp_port_allocator.handle() };

    EXPECT_CALL( driver, write_simr( 0x00 ) );

    network_stack.disable_socket_interrupts();
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()
 *        test case.
 */
struct socketInterruptsAreEnabled_Test_Case {
    /**
     * \brief The SIMR register value.
     */
    std::uint8_t simr;

    /**
     * \brief Socket interrupts are enabled.
     */
    bool socket_interrupts_are_enabled;
};

auto operator<<( std::ostream & stream, socketInterruptsAreEnabled_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".simr = 0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.simr )
                  << ", "
                  << ".socket_interrupts_are_enabled = " << std::boolalpha << test_case.socket_interrupts_are_enabled
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()
 *        test fixture.
 */
class socketInterruptsAreEnabled : public TestWithParam<socketInterruptsAreEnabled_Test_Case> {
};

/**
 * \brief Verify
 *        picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()
 *        works properly.
 */
TEST_P( socketInterruptsAreEnabled, worksProperly )
{
    auto const test_case = GetParam();

    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 126 },
                                              tcp_port_allocator.handle() };

    EXPECT_CALL( driver, read_simr() ).WillOnce( Return( test_case.simr ) );

    ASSERT_EQ( network_stack.socket_interrupts_are_enabled(), test_case.socket_interrupts_are_enabled );
}

/**
 * \brief picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupts_are_enabled()
 *        test cases.
 */
socketInterruptsAreEnabled_Test_Case const socketInterruptsAreEnabled_TEST_CASES[]{
    // clang-format off

    { 0x00, false },
    { 0x01, true  },
    { 0x9C, true  },
    { 0xFF, true  },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, socketInterruptsAreEnabled, ValuesIn( socketInterruptsAreEnabled_TEST_CASES ) );

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::socket_interrupt_context()
 *        works properly.
 */
TEST( socketInterruptContext, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 159 },
                                              tcp_port_allocator.handle() };

    auto const sir = std::uint8_t{ 0b10101011 };

    EXPECT_CALL( driver, read_sir() ).WillOnce( Return( sir ) );

    ASSERT_EQ( network_stack.socket_interrupt_context(), sir );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_ipv4_address()
 *        works properly.
 */
TEST( unreachableIPv4Address, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 174 },
                                              tcp_port_allocator.handle() };

    auto const uipr = Array<std::uint8_t, 4>{ 192, 156, 56, 116 };

    EXPECT_CALL( driver, read_uipr() ).WillOnce( Return( uipr ) );

    ASSERT_EQ( network_stack.unreachable_ipv4_address().as_byte_array(), uipr );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::IP::Network_Stack::unreachable_port() works
 *        properly.
 */
TEST( unreachablePort, worksProperly )
{
    auto driver             = Mock_Driver{};
    auto tcp_port_allocator = Mock_Port_Allocator{};

    auto const network_stack = Network_Stack{ driver,
                                              Mock_Error{ 174 },
                                              tcp_port_allocator.handle() };

    auto const uportr = std::uint16_t{ 31689 };

    EXPECT_CALL( driver, read_uportr() ).WillOnce( Return( uportr ) );

    ASSERT_EQ( network_stack.unreachable_port().as_unsigned_integer(), uportr );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::IP::Network_Stack automated tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
