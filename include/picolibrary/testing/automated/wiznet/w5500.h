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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <vector>

#include "picolibrary/array.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::WIZnet::W5500 {

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::Socket_ID to.
 * \param[in] socket_id The picolibrary::WIZnet::W5500::Socket_ID to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Socket_ID socket_id ) -> std::ostream &
{
    switch ( socket_id ) {
            // clang-format off

        case Socket_ID::_0: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_0";
        case Socket_ID::_1: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_1";
        case Socket_ID::_2: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_2";
        case Socket_ID::_3: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_3";
        case Socket_ID::_4: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_4";
        case Socket_ID::_5: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_5";
        case Socket_ID::_6: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_6";
        case Socket_ID::_7: return stream << "::picolibrary::WIZnet::W5500::Socket_ID::_7";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "socket_id is not a valid ::picolibrary::WIZnet::W5500::Socket_ID"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the
 *            picolibrary::WIZnet::W5500::Socket_Memory_Block to.
 * \param[in] socket_memory_block The picolibrary::WIZnet::W5500::Socket_Memory_Block to
 *            write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Socket_Memory_Block socket_memory_block )
    -> std::ostream &
{
    switch ( socket_memory_block ) {
            // clang-format off

        case Socket_Memory_Block::REGISTERS: return stream << "::picolibrary::WIZnet::W5500::Socket_Memory_Block::REGISTERS";
        case Socket_Memory_Block::TX_BUFFER: return stream << "::picolibrary::WIZnet::W5500::Socket_Memory_Block::TX_BUFFER";
        case Socket_Memory_Block::RX_BUFFER: return stream << "::picolibrary::WIZnet::W5500::Socket_Memory_Block::RX_BUFFER";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "socket_memory_block is not a valid "
        "::picolibrary::WIZnet::W5500::Socket_Memory_Block"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::ARP_Forcing to.
 * \param[in] arp_forcing_configuration The picolibrary::WIZnet::W5500::ARP_Forcing to
 *            write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, ARP_Forcing arp_forcing_configuration ) -> std::ostream &
{
    switch ( arp_forcing_configuration ) {
            // clang-format off

        case ARP_Forcing::DISABLED: return stream << "::picolibrary::WIZnet::W5500::ARP_Forcing::DISABLED";
        case ARP_Forcing::ENABLED:  return stream << "::picolibrary::WIZnet::W5500::ARP_Forcing::ENABLED";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "arp_forcing_configuration is not a valid "
        "::picolibrary::WIZnet::W5500::ARP_Forcing"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::Ping_Blocking to.
 * \param[in] ping_blocking_configuration The picolibrary::WIZnet::W5500::Ping_Blocking to
 *            write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Ping_Blocking ping_blocking_configuration )
    -> std::ostream &
{
    switch ( ping_blocking_configuration ) {
            // clang-format off

        case Ping_Blocking::DISABLED: return stream << "::picolibrary::WIZnet::W5500::Ping_Blocking::DISABLED";
        case Ping_Blocking::ENABLED:  return stream << "::picolibrary::WIZnet::W5500::Ping_Blocking::ENABLED";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "ping_blocking_configuration is not a valid "
        "::picolibrary::WIZnet::W5500::Ping_Blocking"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::PHY_Mode to.
 * \param[in] phy_mode The picolibrary::WIZnet::W5500::PHY_Mode to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, PHY_Mode phy_mode ) -> std::ostream &
{
    switch ( phy_mode ) {
            // clang-format off

        case PHY_Mode::CONFIGURED_BY_HARDWARE:                       return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::CONFIGURED_BY_HARDWARE";
        case PHY_Mode::POWER_DOWN:                                   return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::POWER_DOWN";
        case PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED:  return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::_10BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED";
        case PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED:  return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::_10BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED";
        case PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED: return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_DISABLED";
        case PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED: return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::_100BT_FULL_DUPLEX_AUTO_NEGOTIATION_DISABLED";
        case PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED:  return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::_100BT_HALF_DUPLEX_AUTO_NEGOTIATION_ENABLED";
        case PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED:         return stream << "::picolibrary::WIZnet::W5500::PHY_Mode::ALL_CAPABLE_AUTO_NEGOTIATION_ENABLED";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "phy_mode is not a valid ::picolibrary::WIZnet::W5500::PHY_Mode"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::Link_Status to.
 * \param[in] link_status The picolibrary::WIZnet::W5500::Link_Status to write to the
 *            stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Link_Status link_status ) -> std::ostream &
{
    switch ( link_status ) {
            // clang-format off

        case Link_Status::DOWN: return stream << "::picolibrary::WIZnet::W5500::Link_Status::DOWN";
        case Link_Status::UP:   return stream << "::picolibrary::WIZnet::W5500::Link_Status::UP";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "link_status is not a valid ::picolibrary::WIZnet::W5500::Link_Status"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::Link_Mode to.
 * \param[in] link_mode The picolibrary::WIZnet::W5500::Link_Mode to write to the stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Link_Mode link_mode ) -> std::ostream &
{
    switch ( link_mode ) {
            // clang-format off

        case Link_Mode::HALF_DUPLEX: return stream << "::picolibrary::WIZnet::W5500::Link_Mode::HALF_DUPLEX";
        case Link_Mode::FULL_DUPLEX: return stream << "::picolibrary::WIZnet::W5500::Link_Mode::FULL_DUPLEX";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "link_mode is not a valid ::picolibrary::WIZnet::W5500::Link_Mode"
    };
}

/**
 * \brief Insertion operator.
 *
 * \param[in] stream The stream to write the picolibrary::WIZnet::W5500::Link_Speed to.
 * \param[in] link_speed The picolibrary::WIZnet::W5500::Link_Speed to write to the
 *            stream.
 *
 * \return stream
 */
inline auto operator<<( std::ostream & stream, Link_Speed link_speed ) -> std::ostream &
{
    switch ( link_speed ) {
            // clang-format off

        case Link_Speed::_10_MbPs:  return stream << "::picolibrary::WIZnet::W5500::Link_Speed::_10_MbPs";
        case Link_Speed::_100_MbPs: return stream << "::picolibrary::WIZnet::W5500::Link_Speed::_100_MbPs";

            // clang-format on
    } // switch

    throw std::invalid_argument{
        "link_speed is not a valid ::picolibrary::WIZnet::W5500::Link_Speed"
    };
}

} // namespace picolibrary::WIZnet::W5500

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::WIZnet::W5500::Socket_ID.
 *
 * \return A pseudo-randomly generated picolibrary::WIZnet::W5500::Socket_ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>() -> WIZnet::W5500::Socket_ID
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint_fast8_t>( 0, 7 ) << 5 );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief WIZnet W5500 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500 {

/**
 * \brief Mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    Mock_Communication_Controller() noexcept
    {
    }

    Mock_Communication_Controller(
        SPI::Mock_Controller &,
        SPI::Mock_Controller::Configuration const &,
        SPI::Mock_Device_Selector::Handle ) noexcept
    {
    }

    Mock_Communication_Controller( Mock_Communication_Controller && ) = delete;

    Mock_Communication_Controller( Mock_Communication_Controller const & ) = delete;

    ~Mock_Communication_Controller() noexcept = default;

    auto operator=( Mock_Communication_Controller && ) = delete;

    auto operator=( Mock_Communication_Controller const & ) = delete;

    MOCK_METHOD( std::uint8_t, read, ( ::picolibrary::WIZnet::W5500::Memory_Offset ), ( const ) );
    MOCK_METHOD(
        std::vector<std::uint8_t>,
        read,
        (::picolibrary::WIZnet::W5500::Memory_Offset, std::vector<std::uint8_t>),
        ( const ) );

    void read( ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset, std::uint8_t * begin, std::uint8_t * end ) const
    {
        static_cast<void>( end );

        auto const data = read( memory_offset, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD( void, write, ( ::picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) );
    MOCK_METHOD( void, write, (::picolibrary::WIZnet::W5500::Memory_Offset, std::vector<std::uint8_t>));

    void write( ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        write( memory_offset, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD(
        std::uint8_t,
        read,
        ( ::picolibrary::WIZnet::W5500::Socket_ID,
          ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
          ::picolibrary::WIZnet::W5500::Memory_Offset ),
        ( const ) );
    MOCK_METHOD(
        std::vector<std::uint8_t>,
        read,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>),
        ( const ) );

    void read(
        ::picolibrary::WIZnet::W5500::Socket_ID           socket_id,
        ::picolibrary::WIZnet::W5500::Socket_Memory_Block socket_memory_block,
        ::picolibrary::WIZnet::W5500::Memory_Offset       memory_offset,
        std::uint8_t *                                    begin,
        std::uint8_t *                                    end ) const
    {
        static_cast<void>( end );

        auto const data = read(
            socket_id, socket_memory_block, memory_offset, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD(
        void,
        write,
        ( ::picolibrary::WIZnet::W5500::Socket_ID,
          ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
          ::picolibrary::WIZnet::W5500::Memory_Offset,
          std::uint8_t ) );
    MOCK_METHOD(
        void,
        write,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>));

    void write(
        ::picolibrary::WIZnet::W5500::Socket_ID           socket_id,
        ::picolibrary::WIZnet::W5500::Socket_Memory_Block socket_memory_block,
        ::picolibrary::WIZnet::W5500::Memory_Offset       memory_offset,
        std::uint8_t const *                              begin,
        std::uint8_t const *                              end )
    {
        write( socket_id, socket_memory_block, memory_offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief Mock driver.
 */
class Mock_Driver : public Mock_Communication_Controller {
  public:
    Mock_Driver() noexcept
    {
    }

    Mock_Driver( SPI::Mock_Controller &, SPI::Mock_Controller::Configuration const &, SPI::Mock_Device_Selector::Handle ) noexcept
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( std::uint8_t, read_mr, (), ( const ) );
    MOCK_METHOD( void, write_mr, ( std::uint8_t ) );

    MOCK_METHOD( (Array<std::uint8_t, 4>), read_gar, (), ( const ) );
    MOCK_METHOD( void, write_gar, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Array<std::uint8_t, 4>), read_subr, (), ( const ) );
    MOCK_METHOD( void, write_subr, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Array<std::uint8_t, 6>), read_shar, (), ( const ) );
    MOCK_METHOD( void, write_shar, ( (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( (Array<std::uint8_t, 4>), read_sipr, (), ( const ) );
    MOCK_METHOD( void, write_sipr, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( std::uint16_t, read_intlevel, (), ( const ) );
    MOCK_METHOD( void, write_intlevel, ( std::uint16_t ) );

    MOCK_METHOD( std::uint8_t, read_ir, (), ( const ) );
    MOCK_METHOD( void, write_ir, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_imr, (), ( const ) );
    MOCK_METHOD( void, write_imr, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sir, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read_simr, (), ( const ) );
    MOCK_METHOD( void, write_simr, ( std::uint8_t ) );

    MOCK_METHOD( std::uint16_t, read_rtr, (), ( const ) );
    MOCK_METHOD( void, write_rtr, ( std::uint16_t ) );

    MOCK_METHOD( std::uint8_t, read_rcr, (), ( const ) );
    MOCK_METHOD( void, write_rcr, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_ptimer, (), ( const ) );
    MOCK_METHOD( void, write_ptimer, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_pmagic, (), ( const ) );
    MOCK_METHOD( void, write_pmagic, ( std::uint8_t ) );

    MOCK_METHOD( (Array<std::uint8_t, 6>), read_phar, (), ( const ) );
    MOCK_METHOD( void, write_phar, ( (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( std::uint16_t, read_psid, (), ( const ) );
    MOCK_METHOD( void, write_psid, ( std::uint16_t ) );

    MOCK_METHOD( std::uint16_t, read_pmru, (), ( const ) );
    MOCK_METHOD( void, write_pmru, ( std::uint16_t ) );

    MOCK_METHOD( (Array<std::uint8_t, 4>), read_uipr, (), ( const ) );

    MOCK_METHOD( std::uint16_t, read_uportr, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read_phycfgr, (), ( const ) );
    MOCK_METHOD( void, write_phycfgr, ( std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_versionr, (), ( const ) );

    MOCK_METHOD( std::uint8_t, read_sn_mr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_mr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_cr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_cr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_ir, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_ir, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_sr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( std::uint16_t, read_sn_port, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_port, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Array<std::uint8_t, 6>), read_sn_dhar, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_dhar, ( ::picolibrary::WIZnet::W5500::Socket_ID, (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( (Array<std::uint8_t, 4>), read_sn_dipr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_dipr, ( ::picolibrary::WIZnet::W5500::Socket_ID, (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( std::uint16_t, read_sn_dport, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_dport, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( std::uint16_t, read_sn_mssr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_mssr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_tos, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_tos, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_ttl, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_ttl, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_rxbuf_size, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_rxbuf_size, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_txbuf_size, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_txbuf_size, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint16_t, read_sn_tx_fsr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( std::uint16_t, read_sn_tx_rd, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( std::uint16_t, read_sn_tx_wr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_tx_wr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( std::uint16_t, read_sn_rx_rsr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( std::uint16_t, read_sn_rx_rd, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_rx_rd, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( std::uint16_t, read_sn_rx_wr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );

    MOCK_METHOD( std::uint8_t, read_sn_imr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_imr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD( std::uint16_t, read_sn_frag, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_frag, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint16_t ) );

    MOCK_METHOD( std::uint8_t, read_sn_kpalvtr, ( ::picolibrary::WIZnet::W5500::Socket_ID ), ( const ) );
    MOCK_METHOD( void, write_sn_kpalvtr, ( ::picolibrary::WIZnet::W5500::Socket_ID, std::uint8_t ) );

    MOCK_METHOD(
        std::vector<std::uint8_t>,
        read_rx_buffer,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>),
        ( const ) );

    void read_rx_buffer(
        ::picolibrary::WIZnet::W5500::Socket_ID     socket_id,
        ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset,
        std::uint8_t *                              begin,
        std::uint8_t *                              end ) const
    {
        static_cast<void>( end );

        auto const data = read_rx_buffer( socket_id, memory_offset, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD(
        void,
        write_tx_buffer,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>));

    void write_tx_buffer(
        ::picolibrary::WIZnet::W5500::Socket_ID     socket_id,
        ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset,
        std::uint8_t const *                        begin,
        std::uint8_t const *                        end )
    {
        write_tx_buffer( socket_id, memory_offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H
