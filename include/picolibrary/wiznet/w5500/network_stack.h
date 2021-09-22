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
        // #lizard forgives the length

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

        {
            auto result = m_driver->write_phycfgr(
                static_cast<std::uint8_t>( phy_mode ) | PHYCFGR::Mask::RST );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        return {};
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

  private:
    /**
     * \brief The driver for the W5500 the network stack utilizes.
     */
    Driver * m_driver{};
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_NETWORK_STACK_H
