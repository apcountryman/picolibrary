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
 * \brief picolibrary::WIZnet::W5500::Network_Stack interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_NETWORK_STACK_H
#define PICOLIBRARY_WIZNET_W5500_NETWORK_STACK_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::WIZnet::W5500 {

/**
 * \brief WIZnet W5500 IP network stack.
 *
 * \tparam Driver The W5500 driver implementation. The default W5500 driver implementation
 *         should be used unless a mock W5500 driver implementation us being injected to
 *         support unit testing of this network stack.
 */
template<typename Driver>
class Network_Stack {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Network_Stack() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver for the W5500 the network stack utilizes.
     */
    constexpr Network_Stack( Driver & driver ) noexcept : m_driver{ &driver }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Network_Stack( Network_Stack && source ) noexcept :
        m_driver{ source.m_driver }
    {
        source.m_driver = nullptr;
    }

    /**
     * \brief Destructor.
     */
    ~Network_Stack() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto & operator=( Network_Stack && expression ) noexcept
    {
        if ( &expression != this ) {
            m_driver = expression.m_driver;

            expression.m_driver = nullptr;
        } // if

        return *this;
    }

    /**
     * \brief Configure the PHY.
     *
     * \param[in] phy_mode The desired PHY mode.
     *
     * \return Nothing if PHY configuration succeeded.
     * \return An error code if PHY configuration failed.
     */
    auto configure_phy( PHY_Mode phy_mode ) noexcept -> Result<Void, Error_Code>
    {
        {
            auto result = m_driver->write_phycfgr(
                static_cast<std::uint8_t>( phy_mode ) | PHYCFGR::Mask::RST );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_phycfgr( static_cast<std::uint8_t>( phy_mode ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return m_driver->write_phycfgr( static_cast<std::uint8_t>( phy_mode ) | PHYCFGR::Mask::RST );
    }

    /**
     * \brief Get the PHY mode.
     *
     * \return The PHY mode if getting the PHY mode succeeded.
     * \return An error code if getting the PHY mode failed.
     */
    auto phy_mode() const noexcept -> Result<PHY_Mode, Error_Code>
    {
        auto result = m_driver->read_phycfgr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<PHY_Mode>( result.value() & ( PHYCFGR::Mask::OPMD | PHYCFGR::Mask::OPMDC ) );
    }

    /**
     * \brief Get the link status.
     *
     * \return The link status if getting the link status succeeded.
     * \return An error code if getting the link status failed.
     */
    auto link_status() const noexcept -> Result<Link_Status, Error_Code>
    {
        auto result = m_driver->read_phycfgr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<Link_Status>( result.value() & PHYCFGR::Mask::LNK );
    }

    /**
     * \brief Get the link mode.
     *
     * \return The link mode if getting the link mode succeeded.
     * \return An error code if getting the link mode failed.
     */
    auto link_mode() const noexcept -> Result<Link_Mode, Error_Code>
    {
        auto result = m_driver->read_phycfgr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<Link_Mode>( result.value() & PHYCFGR::Mask::DPX );
    }

    /**
     * \brief Get the link speed.
     *
     * \return The link speed if getting the link speed succeeded.
     * \return An error code if getting the link speed failed.
     */
    auto link_speed() const noexcept -> Result<Link_Speed, Error_Code>
    {
        auto result = m_driver->read_phycfgr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<Link_Speed>( result.value() & PHYCFGR::Mask::SPD );
    }

    /**
     * \brief Configure ping blocking.
     *
     * \param[in] ping_blocking_configuration The desired ping blocking configuration.
     *
     * \return Nothing if ping blocking configuration succeeded.
     * \return An error code if ping blocking configuration failed.
     */
    auto configure_ping_blocking( Ping_Blocking ping_blocking_configuration ) noexcept
        -> Result<Void, Error_Code>
    {
        auto result = m_driver->read_mr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return m_driver->write_mr(
            ( result.value() & ~MR::Mask::PB )
            | static_cast<std::uint8_t>( ping_blocking_configuration ) );
    }

    /**
     * \brief Get the ping blocking configuration.
     *
     * \return The ping blocking configuration if getting the ping blocking configuration
     *         succeeded.
     * \return An error code if getting the ping blocking configuration failed.
     */
    auto ping_blocking_configuration() const noexcept -> Result<Ping_Blocking, Error_Code>
    {
        auto result = m_driver->read_mr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<Ping_Blocking>( result.value() & MR::Mask::PB );
    }

    /**
     * \brief Configure ARP forcing.
     *
     * \param[in] arp_forcing_configuration The desired ARP forcing configuration.
     *
     * \return Nothing if ARP forcing configuration succeeded.
     * \return An error code if ARP forcing configuration failed.
     */
    auto configure_arp_forcing( ARP_Forcing arp_forcing_configuration ) noexcept
        -> Result<Void, Error_Code>
    {
        auto result = m_driver->read_mr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return m_driver->write_mr(
            ( result.value() & ~MR::Mask::FARP )
            | static_cast<std::uint8_t>( arp_forcing_configuration ) );
    }

    /**
     * \brief Get the ARP forcing configuration.
     *
     * \return The ARP forcing configuration if getting the ARP forcing configuration
     *         succeeded.
     * \return An error code if getting the ARP forcing configuration failed.
     */
    auto arp_forcing_configuration() const noexcept -> Result<ARP_Forcing, Error_Code>
    {
        auto result = m_driver->read_mr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<ARP_Forcing>( result.value() & MR::Mask::FARP );
    }

    /**
     * \brief Configure retransmission.
     *
     * \param[in] retry_time The desired retry time (value written to the RTR register).
     * \param[in] retry_count The desired retry count (value written to the RCR register).
     *
     * \return Nothing if retransmission configuration succeeded.
     * \return An error code if retransmission configuration failed.
     */
    auto configure_retransmission( std::uint16_t retry_time, std::uint8_t retry_count ) noexcept
        -> Result<Void, Error_Code>
    {
        {
            auto result = m_driver->write_rtr( retry_time );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return m_driver->write_rcr( retry_count );
    }

    /**
     * \brief Get the retry time (RTR register value).
     *
     * \return The retry time if getting the retry time succeeded.
     * \return An error code if getting the retry time failed.
     */
    auto retry_time() const noexcept
    {
        return m_driver->read_rtr();
    }

    /**
     * \brief Get the retry count (RCR register value).
     *
     * \return The retry count if getting the retry count succeeded.
     * \return An error code if getting the retry count failed.
     */
    auto retry_count() const noexcept
    {
        return m_driver->read_rcr();
    }

    /**
     * \brief Configure socket buffers.
     *
     * \param[in] buffer_size The desired socket buffer size. Must be at least 2 KiB. The
     *            socket buffer size will determine the number of available sockets (2 KiB
     *            -> 8 sockets, 4 KiB -> 4 sockets, 8 KiB -> 2 sockets, 16 KiB -> 1
     *            socket).
     *
     * \return Nothing if socket buffers configuration succeeded.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if buffer_size is less than 2
     *         KiB or buffer_size is not a valid socket buffer size.
     * \return An error code if socket buffers configuration failed for any other reason.
     */
    auto configure_socket_buffers( Buffer_Size buffer_size ) noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        auto available_sockets = std::uint8_t{};

        switch ( buffer_size ) {
            case Buffer_Size::_2_KIB: available_sockets = 16 / 2; break;
            case Buffer_Size::_4_KIB: available_sockets = 16 / 4; break;
            case Buffer_Size::_8_KIB: available_sockets = 16 / 8; break;
            case Buffer_Size::_16_KIB: available_sockets = 16 / 16; break;
            default: return Generic_Error::INVALID_ARGUMENT;
        } // switch

        for ( auto socket = std::uint8_t{}; socket < available_sockets; ++socket ) {
            {
                auto result = m_driver->write_sn_rxbuf_size(
                    static_cast<Socket_ID>( socket ), static_cast<std::uint8_t>( buffer_size ) );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            {
                auto result = m_driver->write_sn_txbuf_size(
                    static_cast<Socket_ID>( socket ), static_cast<std::uint8_t>( buffer_size ) );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }
        } // for

        for ( auto socket = available_sockets; socket < SOCKETS; ++socket ) {
            {
                auto result = m_driver->write_sn_rxbuf_size(
                    static_cast<Socket_ID>( socket ),
                    static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            {
                auto result = m_driver->write_sn_txbuf_size(
                    static_cast<Socket_ID>( socket ),
                    static_cast<std::uint8_t>( Buffer_Size::_0_KIB ) );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }
        } // for

        return {};
    }

    /**
     * \brief Get the socket buffer size.
     *
     * \return The socket buffer size if getting the socket buffer size succeeded.
     * \return An error code if getting the socket buffer size failed.
     */
    auto socket_buffer_size() const noexcept -> Result<Buffer_Size, Error_Code>
    {
        auto result = m_driver->read_sn_rxbuf_size( Socket_ID::_0 );
        if ( result.is_error() ) {
            return result.error();
        } // if

        return static_cast<Buffer_Size>( result.value() );
    }

    /**
     * \brief Configure the W5500's MAC address.
     *
     * \param[in] address The desired MAC address.
     *
     * \return Nothing if W5500 MAC address configuration succeeded.
     * \return An error code if W5500 MAC address configuration failed.
     */
    auto configure_mac_address( MAC_Address const & address ) noexcept
    {
        return m_driver->write_shar( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's MAC address.
     *
     * \return The W5500's MAC address if getting the W5500's MAC address succeeded.
     * \return An error code if getting the W5500's MAC address failed.
     */
    auto mac_address() const noexcept -> Result<MAC_Address, Error_Code>
    {
        auto result = m_driver->read_shar();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return MAC_Address{ result.value() };
    }

    /**
     * \brief Configure the W5500's IP address.
     *
     * \param[in] address The desired IP address.
     *
     * \return Nothing if W5500 IP address configuration succeeded.
     * \return An error code if W5500 IP address configuration failed.
     */
    auto configure_ip_address( IPv4::Address const & address ) noexcept
    {
        return m_driver->write_sipr( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's IP address.
     *
     * \return The W5500's IP address if getting the W5500's IP address succeeded.
     * \return An error code if getting the W5500's IP address failed.
     */
    auto ip_address() const noexcept -> Result<IPv4::Address, Error_Code>
    {
        auto result = m_driver->read_sipr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return IPv4::Address{ result.value() };
    }

    /**
     * \brief Configure the W5500's gateway IP address.
     *
     * \param[in] address The desired gateway IP address.
     *
     * \return Nothing if W5500 gateway IP address configuration succeeded.
     * \return An error code if W5500 gateway IP address configuration failed.
     */
    auto configure_gateway_ip_address( IPv4::Address const & address ) noexcept
    {
        return m_driver->write_gar( address.as_byte_array() );
    }

    /**
     * \brief Get the W5500's gateway IP address.
     *
     * \return The W5500's gateway IP address if getting the W5500's gateway IP address
     *         succeeded.
     * \return An error code if getting the W5500's gateway IP address failed.
     */
    auto gateway_ip_address() const noexcept -> Result<IPv4::Address, Error_Code>
    {
        auto result = m_driver->read_gar();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return IPv4::Address{ result.value() };
    }

    /**
     * \brief Configure the W5500's subnet mask.
     *
     * \param[in] subnet_mask The desired subnet mask.
     *
     * \return Nothing if W5500 subnet mask configuration succeeded.
     * \return An error code if W5500 subnet mask configuration failed.
     */
    auto configure_subnet_mask( IPv4::Address const & subnet_mask ) noexcept
    {
        return m_driver->write_subr( subnet_mask.as_byte_array() );
    }

    /**
     * \brief Get the W5500's subnet mask.
     *
     * \return The W5500's subnet mask if getting the W5500's subnet mask succeeded.
     * \return An error code if getting the W5500's subnet mask failed.
     */
    auto subnet_mask() const noexcept -> Result<IPv4::Address, Error_Code>
    {
        auto result = m_driver->read_subr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return IPv4::Address{ result.value() };
    }

  private:
    /**
     * \brief The driver for the W5500 the network stack utilizes.
     */
    Driver * m_driver{};
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_NETWORK_STACK_H
