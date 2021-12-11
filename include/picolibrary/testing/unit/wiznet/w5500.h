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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket ID.
 *
 * \return A pseudo-randomly generated WIZnet W5500 socket ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>()
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint8_t>( 0, 7 ) << 5 );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 region.
 *
 * \return A pseudo-randomly generated WIZnet W5500 region.
 */
template<>
inline auto random<WIZnet::W5500::Region>()
{
    return static_cast<WIZnet::W5500::Region>( random<std::uint8_t>( 0b01, 0b11 ) << 3 );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 ping blocking configuration.
 *
 * \return A pseudo-randomly generated WIZnet W5500 ping blocking configuration.
 */
template<>
inline auto random<WIZnet::W5500::Ping_Blocking>()
{
    return random<bool>() ? WIZnet::W5500::Ping_Blocking::DISABLED
                          : WIZnet::W5500::Ping_Blocking::ENABLED;
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 ARP forcing configuration.
 *
 * \return A pseudo-randomly generated WIZnet W5500 ARP forcing configuration.
 */
template<>
inline auto random<WIZnet::W5500::ARP_Forcing>()
{
    return random<bool>() ? WIZnet::W5500::ARP_Forcing::DISABLED : WIZnet::W5500::ARP_Forcing::ENABLED;
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 PHY mode.
 *
 * \return A pseudo-randomly generated WIZnet W5500 PHY mode.
 */
template<>
inline auto random<WIZnet::W5500::PHY_Mode>()
{
    return static_cast<WIZnet::W5500::PHY_Mode>( random<std::uint8_t>( 0b0000, 0b1111 ) << 3 );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 link status.
 *
 * \return A pseudo-randomly generated WIZnet W5500 link status.
 */
template<>
inline auto random<WIZnet::W5500::Link_Status>()
{
    return random<bool>() ? WIZnet::W5500::Link_Status::DOWN : WIZnet::W5500::Link_Status::UP;
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 link mode.
 *
 * \return A pseudo-randomly generated WIZnet W5500 link mode.
 */
template<>
inline auto random<WIZnet::W5500::Link_Mode>()
{
    return random<bool>() ? WIZnet::W5500::Link_Mode::HALF_DUPLEX : WIZnet::W5500::Link_Mode::FULL_DUPLEX;
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 link speed.
 *
 * \return A pseudo-randomly generated WIZnet W5500 link speed.
 */
template<>
inline auto random<WIZnet::W5500::Link_Speed>()
{
    return random<bool>() ? WIZnet::W5500::Link_Speed::_10_MBPS : WIZnet::W5500::Link_Speed::_100_MBPS;
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket buffer size within the specified
 *        range.
 *
 * \param[in] min The lower bound of the allowable range.
 * \param[in] max The upper bound of the allowable range.
 *
 * \return A pseudo-random WIZnet W5500 socket buffer size in the range [min,max].
 */
template<>
inline auto random<WIZnet::W5500::Buffer_Size>( WIZnet::W5500::Buffer_Size min, WIZnet::W5500::Buffer_Size max )
{
    auto const shift = []( WIZnet::W5500::Buffer_Size buffer_size ) -> std::uint8_t {
        switch ( buffer_size ) {
            case WIZnet::W5500::Buffer_Size::_0_KIB: return 0;
            case WIZnet::W5500::Buffer_Size::_1_KIB: return 1;
            case WIZnet::W5500::Buffer_Size::_2_KIB: return 2;
            case WIZnet::W5500::Buffer_Size::_4_KIB: return 3;
            case WIZnet::W5500::Buffer_Size::_8_KIB: return 4;
            case WIZnet::W5500::Buffer_Size::_16_KIB: return 5;
            default: std::abort();
        } // switch
    };

    return static_cast<WIZnet::W5500::Buffer_Size>(
        ( 1 << random<std::uint8_t>( shift( min ), shift( max ) ) ) >> 1 );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket buffer size greater than or equal
 *        to a minimum socket buffer size.
 *
 * \param[in] min The lower bound of the allowable range.
 *
 * \return A pseudo-random WIZnet W5500 socket buffer size in the range
 *         [min,picolibrary::WIZnet::W5500::Buffer_Size::_16_KIB].
 */
template<>
inline auto random<WIZnet::W5500::Buffer_Size>( WIZnet::W5500::Buffer_Size min )
{
    return random<WIZnet::W5500::Buffer_Size>( min, WIZnet::W5500::Buffer_Size::_16_KIB );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket buffer size.
 *
 * \return A pseudo-random WIZnet W5500 socket buffer size in the range
 *         [picolibrary::WIZnet::W5500::Buffer_Size::_0_KIB,picolibrary::WIZnet::W5500::Buffer_Size::_16_KIB].
 */
template<>
inline auto random<WIZnet::W5500::Buffer_Size>()
{
    return random<WIZnet::W5500::Buffer_Size>(
        WIZnet::W5500::Buffer_Size::_0_KIB, WIZnet::W5500::Buffer_Size::_16_KIB );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 no delayed ACK configuration.
 *
 * \return A pseudo-randomly generated WIZnet W5500 no delayed ACK configuration.
 */
template<>
inline auto random<WIZnet::W5500::No_Delayed_ACK>()
{
    return random<bool>() ? WIZnet::W5500::No_Delayed_ACK::DISABLED
                          : WIZnet::W5500::No_Delayed_ACK::ENABLED;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief WIZnet W5500 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::WIZnet::W5500 {

/**
 * \brief Socket ID.
 */
using Socket_ID = ::picolibrary::WIZnet::W5500::Socket_ID;

/**
 * \brief Region.
 */
using Region = ::picolibrary::WIZnet::W5500::Region;

/**
 * \brief WIZnet W5500 mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    Mock_Communication_Controller() = default;

    Mock_Communication_Controller( SPI::Mock_Controller &, SPI::Mock_Controller::Configuration, SPI::Mock_Device_Selector::Handle )
    {
    }

    Mock_Communication_Controller( Mock_Communication_Controller && ) = delete;

    Mock_Communication_Controller( Mock_Communication_Controller const & ) = delete;

    ~Mock_Communication_Controller() noexcept = default;

    auto operator=( Mock_Communication_Controller && ) = delete;

    auto operator=( Mock_Communication_Controller const & ) = delete;

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( std::uint16_t ), ( const ) );
    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (std::uint16_t, std::vector<std::uint8_t>),
        ( const ) );

    auto read( std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( offset, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint16_t, std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write, (std::uint16_t, std::vector<std::uint8_t>));

    auto write( std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( offset, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( Socket_ID, Region, std::uint16_t ), ( const ) );
    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (Socket_ID, Region, std::uint16_t, std::vector<std::uint8_t>),
        ( const ) );

    auto read( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( socket_id, region, offset, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( Socket_ID, Region, std::uint16_t, std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write, (Socket_ID, Region, std::uint16_t, std::vector<std::uint8_t>));

    auto write( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( socket_id, region, offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

/**
 * \brief WIZnet W5500 mock driver.
 */
class Mock_Driver : public Mock_Communication_Controller {
  public:
    Mock_Driver() = default;

    Mock_Driver( SPI::Mock_Controller &, SPI::Mock_Controller::Configuration, SPI::Mock_Device_Selector::Handle )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_mr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_mr, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 4>, Error_Code>), read_gar, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_gar, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 4>, Error_Code>), read_subr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_subr, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 6>, Error_Code>), read_shar, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_shar, ( (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 4>, Error_Code>), read_sipr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sipr, ( (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_intlevel, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_intlevel, ( std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_ir, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_ir, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_imr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_imr, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sir, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_simr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_simr, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_rtr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_rtr, ( std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_rcr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_rcr, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_ptimer, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_ptimer, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_pmagic, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_pmagic, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 6>, Error_Code>), read_phar, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_phar, ( (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_psid, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_psid, ( std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_pmru, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_pmru, ( std::uint16_t ) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 4>, Error_Code>), read_uipr, (), ( const ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_uportr, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_phycfgr, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_phycfgr, ( std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_versionr, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_mr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_mr, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_cr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_cr, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_ir, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_ir, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_sr, ( Socket_ID ), ( const ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_port, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_port, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 6>, Error_Code>), read_sn_dhar, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_dhar, ( Socket_ID, (Array<std::uint8_t, 6> const &)) );

    MOCK_METHOD( (Result<Array<std::uint8_t, 4>, Error_Code>), read_sn_dipr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_dipr, ( Socket_ID, (Array<std::uint8_t, 4> const &)) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_dport, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_dport, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_mssr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_mssr, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_tos, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_tos, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_ttl, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_ttl, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_rxbuf_size, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_rxbuf_size, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_txbuf_size, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_txbuf_size, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_tx_fsr, ( Socket_ID ), ( const ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_tx_rd, ( Socket_ID ), ( const ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_tx_wr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_tx_wr, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_rx_rsr, ( Socket_ID ), ( const ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_rx_rd, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_rx_rd, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_rx_wr, ( Socket_ID ), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_imr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_imr, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), read_sn_frag, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_frag, ( Socket_ID, std::uint16_t ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read_sn_kpalvtr, ( Socket_ID ), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), write_sn_kpalvtr, ( Socket_ID, std::uint8_t ) );

    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (Socket_ID, std::uint16_t, std::vector<std::uint8_t>),
        ( const ) );

    auto read( Socket_ID socket_id, std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( socket_id, offset, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, (Socket_ID, std::uint16_t, std::vector<std::uint8_t>));

    auto write( Socket_ID socket_id, std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( socket_id, offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H
