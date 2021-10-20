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
#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
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
 *         should be used unless a mock W5500 driver implementation is being injected to
 *         support unit testing of this network stack.
 */
template<typename Driver>
class Network_Stack {
  public:
    /**
     * \brief TCP over IP client socket.
     */
    class TCP_Client {
      public:
        /**
         * \brief The unsigned integral type used to report transmit/receive buffer
         *        information.
         */
        using Size = std::uint16_t;

        /**
         * \brief Constructor.
         */
        constexpr TCP_Client() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] driver The driver for the W5500 the network stack the socket is
         *            associated with utilizes.
         * \param[in] socket_id The socket's socket ID.
         * \param[in] network_stack The network stack the socket is associated with.
         */
        constexpr TCP_Client( Driver & driver, Socket_ID socket_id, Network_Stack & network_stack ) noexcept :
            m_driver{ &driver },
            m_socket_id{ socket_id },
            m_network_stack{ &network_stack }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        constexpr TCP_Client( TCP_Client && source ) noexcept :
            m_driver{ source.m_driver },
            m_socket_id{ source.m_socket_id },
            m_state{ source.m_state },
            m_network_stack{ source.m_network_stack }
        {
            source.m_driver        = nullptr;
            source.m_network_stack = nullptr;
        }

        TCP_Client( TCP_Client const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~TCP_Client() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        constexpr auto & operator=( TCP_Client && expression ) noexcept
        {
            if ( &expression != this ) {
                m_driver        = expression.m_driver;
                m_socket_id     = expression.m_socket_id;
                m_state         = expression.m_state;
                m_network_stack = expression.m_network_stack;

                expression.m_driver        = nullptr;
                expression.m_network_stack = nullptr;
            } // if

            return *this;
        }

        auto operator=( TCP_Client const & ) = delete;

        /**
         * \brief Get the socket's socket ID.
         *
         * \return The socket's socket ID.
         */
        constexpr auto socket_id() const noexcept
        {
            return m_socket_id;
        }

        /**
         * \brief Get the socket's socket interrupt mask (mask to be used when checking
         *        the network stack's socket interrupt context).
         *
         * \return The socket's socket interrupt mask.
         */
        constexpr auto socket_interrupt_mask() const noexcept
        {
            return static_cast<std::uint8_t>(
                1 << ( static_cast<std::uint8_t>( m_socket_id ) >> Control_Byte::Bit::SOCKET ) );
        }

        /**
         * \brief Enable interrupts.
         *
         * \param[in] mask The mask identifying the interrupts to enable.
         *
         * \return Nothing if enabling interrupts succeeded.
         * \return An error code if enabling interrupts failed.
         */
        auto enable_interrupts( std::uint8_t mask ) noexcept -> Result<Void, Error_Code>
        {
            auto result = m_driver->read_sn_imr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            return m_driver->write_sn_imr( m_socket_id, result.value() | mask );
        }

        /**
         * \brief Disable interrupts.
         *
         * \param[in] mask The mask identifying the interrupts to disable.
         *
         * \return Nothing if disabling interrupts succeeded.
         * \return An error code if disabling interrupts failed.
         */
        auto disable_interrupts( std::uint8_t mask ) noexcept -> Result<Void, Error_Code>
        {
            auto result = m_driver->read_sn_imr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            return m_driver->write_sn_imr( m_socket_id, result.value() & ~mask );
        }

        /**
         * \brief Disable all interrupts.
         *
         * \return Nothing if disabling all interrupts succeeded.
         * \return An error code if disabling all interrupts failed.
         */
        auto disable_interrupts() noexcept
        {
            return m_driver->write_sn_imr( m_socket_id, 0 );
        }

        /**
         * \brief Get a mask identifying the interrupts that are enabled.
         *
         * \return A mask identifying the interrupts that are enabled if getting a mask
         *         identifying the interrupts that are enabled succeeded.
         * \return An error code if getting a mask identifying the interrupts that are
         *         enabled failed.
         */
        auto enabled_interrupts() const noexcept
        {
            return m_driver->read_sn_imr( m_socket_id );
        }

        /**
         * \brief Get the interrupt context (SN_IR register value).
         *
         * \return The interrupt context if getting the interrupt context succeeded.
         * \return An error code if getting the interrupt context failed.
         */
        auto interrupt_context() const noexcept
        {
            return m_driver->read_sn_ir( m_socket_id );
        }

        /**
         * \brief Configure the socket's no delayed ACK usage.
         *
         * \param[in] no_delayed_ack_configuration The desired no delayed ACK
         *            configuration.
         *
         * \return Nothing if configuring the socket's no delayed ACK usage succeeded.
         * \return An error code if configuring the socket's no delayed ACK usage failed.
         */
        auto configure_no_delayed_ack( No_Delayed_ACK no_delayed_ack_configuration ) noexcept
            -> Result<Void, Error_Code>
        {
            auto result = m_driver->read_sn_mr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            return m_driver->write_sn_mr(
                m_socket_id,
                ( result.value() & ~SN_MR::Mask::ND )
                    | static_cast<std::uint8_t>( no_delayed_ack_configuration ) );
        }

        /**
         * \brief Get the socket's no delayed ACK configuration.
         *
         * \return The socket's no delayed ACK configuration if getting the socket's no
         *         delayed ACK configuration succeeded.
         * \return An error code if getting the socket's no delayed ACK configuration
         *         failed.
         */
        auto no_delayed_ack_configuration() const noexcept -> Result<No_Delayed_ACK, Error_Code>
        {
            auto result = m_driver->read_sn_mr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            return static_cast<No_Delayed_ACK>( result.value() & SN_MR::Mask::ND );
        }

        /**
         * \brief Configure the socket's maximum segment size.
         *
         * \param[in] maximum_segment_size The desired maximum segment size.
         *
         * \return Nothing if configuring the socket's maximum segment size succeeded.
         * \return An error code if configuring the socket's maximum segment size failed.
         */
        auto configure_maximum_segment_size( std::uint16_t maximum_segment_size ) noexcept
        {
            return m_driver->write_sn_mssr( m_socket_id, maximum_segment_size );
        }

        /**
         * \brief Get the socket's maximum segment size.
         *
         * \return The socket's maximum segment size if getting the socket's maximum
         *         segment size succeeded.
         * \return An error code if getting the socket's maximum segment size failed.
         */
        auto maximum_segment_size() const noexcept
        {
            return m_driver->read_sn_mssr( m_socket_id );
        }

        /**
         * \brief Configure the socket's IPv4 packet time to live field value.
         *
         * \param[in] time_to_live The desired IPv4 packet time to live field value.
         *
         * \return Nothing if configuring the socket's IPv4 packet time to live field
         *         value succeeded.
         * \return An error code if configuring the socket's IPv4 packet time to live
         *         field value failed.
         */
        auto configure_time_to_live( std::uint8_t time_to_live ) noexcept
        {
            return m_driver->write_sn_ttl( m_socket_id, time_to_live );
        }

        /**
         * \brief Get the socket's IPv4 packet time to live field value.
         *
         * \return The socket's IPv4 packet time to live field value if getting the
         *         socket's IPv4 packet time to live field value succeeded.
         * \return An error code if getting the socket's IPv4 packet time to live field
         *         value failed.
         */
        auto time_to_live() const noexcept
        {
            return m_driver->read_sn_ttl( m_socket_id );
        }

        /**
         * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
         *        register value.
         *
         * \param[in] keepalive_period The desired keepalive packet transmission period.
         *
         * \return Nothing if configuring the socket's keepalive packet transmission
         *         period succeeded.
         * \return An error code if configuring the socket's keepalive packet transmission
         *         period failed.
         */
        auto configure_keepalive_period( std::uint8_t keepalive_period ) noexcept
        {
            return m_driver->write_sn_kpalvtr( m_socket_id, keepalive_period );
        }

        /**
         * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR
         *        register value).
         *
         * \return The socket's keepalive packet transmission period if getting the
         *         socket's keepalive packet transmission period succeeded.
         * \return An error code if getting the socket's keepalive packet transmission
         *         period failed.
         */
        auto keepalive_period() const noexcept
        {
            return m_driver->read_sn_kpalvtr( m_socket_id );
        }

        /**
         * \brief Bind the socket to a specific local endpoint.
         *
         * \param[in] endpoint The local endpoint to bind the socket to.
         *
         * \return Nothing if binding the socket to the local endpoint succeeded.
         * \return picolibrary::Generic_Error::INVALID_ARGUMENT if endpoint is not a valid
         *         local endpoint.
         * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket has already been
         *         bound to a local endpoint.
         * \return picolibrary::Generic_Error::ENDPOINT_IN_USE if endpoint is already in
         *         use.
         * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED if an ephemeral
         *         port was requested and no ephemeral ports are available.
         * \return An error code if binding the socket to the local endpoint failed for
         *         any other reason.
         */
        auto bind( IP::TCP::Endpoint const & endpoint = IP::TCP::Endpoint{} ) noexcept
            -> Result<Void, Error_Code>
        {
            // #lizard forgives the length
            // #lizard forgives the complexity

            if ( m_state != State::CONSTRUCTED ) {
                return Generic_Error::LOGIC_ERROR;
            } // if

            switch ( endpoint.address().version() ) {
                case IP::Version::UNSPECIFIED: break;
                case IP::Version::_4: break;
                default: return Generic_Error::INVALID_ARGUMENT;
            } // switch

            if ( not endpoint.address().is_any() ) {
                auto result = m_driver->read_sipr();
                if ( result.is_error() ) {
                    return result.error();
                } // if

                if ( endpoint.address().ipv4().as_byte_array() != result.value() ) {
                    return Generic_Error::INVALID_ARGUMENT;
                } // if
            }     // if

            if ( endpoint.port().is_any() and not m_network_stack->m_tcp_ephemeral_port_allocation_enabled ) {
                return Generic_Error::EPHEMERAL_PORTS_EXHAUSTED;
            } // if

            SN_PORT::Type used_ports[ SOCKETS ];

            for ( auto socket = std::uint8_t{}; socket < m_network_stack->m_available_sockets; ++socket ) {
                auto const socket_id = static_cast<Socket_ID>( socket << Control_Byte::Bit::SOCKET );

                Protocol protocol;
                {
                    auto result = m_driver->read_sn_mr( socket_id );
                    if ( result.is_error() ) {
                        return result.error();
                    } // if

                    protocol = static_cast<Protocol>( result.value() & SN_MR::Mask::P );
                }

                SN_PORT::Type port;
                {
                    auto result = m_driver->read_sn_port( socket_id );
                    if ( result.is_error() ) {
                        return result.error();
                    } // if

                    port = result.value();
                }

                used_ports[ socket ] = protocol == Protocol::TCP ? port : 0;
            } // for

            auto const is_available = [ &used_ports, available_sockets = m_network_stack->m_available_sockets ](
                                          SN_PORT::Type port ) noexcept -> bool {
                for ( auto socket = std::uint8_t{}; socket < available_sockets; ++socket ) {
                    if ( used_ports[ socket ] == port ) {
                        return false;
                    } // if
                }     // for

                return true;
            };

            SN_PORT::Type port;

            if ( endpoint.port().is_any() ) {
                port = 0;

                auto const ephemeral_port_min = m_network_stack->m_tcp_ephemeral_port_min.as_unsigned_integer();
                auto const ephemeral_port_max = m_network_stack->m_tcp_ephemeral_port_max.as_unsigned_integer();

                for ( auto ephemeral_port = ephemeral_port_min; ephemeral_port <= ephemeral_port_max;
                      ++ephemeral_port ) {
                    if ( is_available( ephemeral_port ) ) {
                        port = ephemeral_port;

                        break;
                    } // if
                }     // for

                if ( not port ) {
                    return Generic_Error::EPHEMERAL_PORTS_EXHAUSTED;
                } // if
            } else {
                if ( not is_available( endpoint.port().as_unsigned_integer() ) ) {
                    return Generic_Error::ENDPOINT_IN_USE;
                } // if

                port = endpoint.port().as_unsigned_integer();
            } // else

            {
                auto result = m_driver->write_sn_port( m_socket_id, port );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            m_state = State::BOUND;

            return {};
        }

      private:
        /**
         * \brief Socket state.
         */
        enum class State : std::uint_fast8_t {
            CONSTRUCTED, ///< Constructed.
            BOUND,       ///< Bound.
        };

        /**
         * \brief The driver for the W5500 the network stack the socket is associated with
         *        utilizes.
         */
        Driver * m_driver{};

        /**
         * \brief The socket's socket ID.
         */
        Socket_ID m_socket_id{};

        /**
         * \brief The socket's state.
         */
        State m_state{ State::CONSTRUCTED };

        /**
         * \brief The network stack the socket is associated with.
         */
        Network_Stack * m_network_stack{};
    };

    /**
     * \brief Constructor.
     */
    constexpr Network_Stack() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver for the W5500 the network stack utilizes.
     * \param[in] nonresponsive_device_error The error code to return when an operation
     *            fails due to the W5500 being nonresponsive.
     */
    constexpr Network_Stack( Driver & driver, Error_Code const & nonresponsive_device_error ) noexcept :
        m_driver{ &driver },
        m_nonresponsive_device_error{ nonresponsive_device_error }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Network_Stack( Network_Stack && source ) noexcept :
        m_driver{ source.m_driver },
        m_nonresponsive_device_error{ source.m_nonresponsive_device_error },
        m_available_sockets{ source.m_available_sockets },
        m_tcp_ephemeral_port_allocation_enabled{ source.m_tcp_ephemeral_port_allocation_enabled },
        m_tcp_ephemeral_port_min{ source.m_tcp_ephemeral_port_min },
        m_tcp_ephemeral_port_max{ source.m_tcp_ephemeral_port_max }
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
            m_driver                     = expression.m_driver;
            m_nonresponsive_device_error = expression.m_nonresponsive_device_error;
            m_available_sockets          = expression.m_available_sockets;
            m_tcp_ephemeral_port_allocation_enabled = expression.m_tcp_ephemeral_port_allocation_enabled;
            m_tcp_ephemeral_port_min                = expression.m_tcp_ephemeral_port_min;
            m_tcp_ephemeral_port_max                = expression.m_tcp_ephemeral_port_max;

            expression.m_driver = nullptr;
        } // if

        return *this;
    }

    /**
     * \brief Get the error code that is returned when an operation fails due to the W5500
     *        being nonresponsive.
     */
    constexpr auto const & nonresponsive_device_error() const noexcept
    {
        return m_nonresponsive_device_error;
    }

    /**
     * \brief Check if the W5500 is responsive by reading the VERSIONR register.
     *
     * \return Nothing if the W5500 is responsive.
     * \return picolibrary::WIZnet::W5500::Network_Stack<Driver>::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if the check failed for any other reason.
     */
    auto ping_w5500() const noexcept -> Result<Void, Error_Code>
    {
        auto result = m_driver->read_versionr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        if ( result.value() != VERSION ) {
            return m_nonresponsive_device_error;
        } // if

        return {};
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

        m_available_sockets = available_sockets;

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

    /**
     * \brief Configure the W5500's interrupt assert wait time.
     *
     * \param[in] interrupt_assert_wait_time The desired interrupt assert wait time
     *            (value written to the INTLEVEL register).
     *
     * \return Nothing if W5500 interrupt assert wait time configuration succeeded.
     * \return An error code if W5500 interrupt assert wait time configuration failed.
     */
    auto configure_interrupt_assert_wait_time( std::uint16_t interrupt_assert_wait_time ) noexcept
    {
        return m_driver->write_intlevel( interrupt_assert_wait_time );
    }

    /**
     * \brief Get the W5500's interrupt assert wait time (INTLEVEL register value).
     *
     * \return The W5500's interrupt assert wait time if getting the W5500's interrupt
     *         assert wait time succeeded.
     * \return An error code if getting the W5500's interrupt assert wait time failed.
     */
    auto interrupt_assert_wait_time() const noexcept
    {
        return m_driver->read_intlevel();
    }

    /**
     * \brief Enable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to enable.
     *
     * \return Nothing if enabling interrupts succeeded.
     * \return An error code if enabling interrupts failed.
     */
    auto enable_interrupts( std::uint8_t mask ) noexcept -> Result<Void, Error_Code>
    {
        auto result = m_driver->read_imr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return m_driver->write_imr( result.value() | mask );
    }

    /**
     * \brief Disable interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to disable.
     *
     * \return Nothing if disabling interrupts succeeded.
     * \return An error code if disabling interrupts failed.
     */
    auto disable_interrupts( std::uint8_t mask ) noexcept -> Result<Void, Error_Code>
    {
        auto result = m_driver->read_imr();
        if ( result.is_error() ) {
            return result.error();
        } // if

        return m_driver->write_imr( result.value() & ~mask );
    }

    /**
     * \brief Disable all interrupts.
     *
     * \return Nothing if disabling all interrupts succeeded.
     * \return An error code if disabling all interrupts failed.
     */
    auto disable_interrupts() noexcept
    {
        return m_driver->write_imr( 0 );
    }

    /**
     * \brief Get a mask identifying the interrupts that are enabled.
     *
     * \return A mask identifying the interrupts that are enabled if getting a mask
     *         identifying the interrupts that are enabled succeeded.
     * \return An error code if getting a mask identifying the interrupts that are enabled
     *         failed.
     */
    auto enabled_interrupts() const noexcept
    {
        return m_driver->read_imr();
    }

    /**
     * \brief Get the interrupt context (IR register value).
     *
     * \return The interrupt context if getting the interrupt context succeeded.
     * \return An error code if getting the interrupt context failed.
     */
    auto interrupt_context() const noexcept
    {
        return m_driver->read_ir();
    }

    /**
     * \brief Enable socket interrupts.
     *
     * \return Nothing if enabling socket interrupts succeeded.
     * \return An error code if enabling socket interrupts failed.
     */
    auto enable_socket_interrupts() noexcept
    {
        return m_driver->write_simr( 0xFF );
    }

    /**
     * \brief Disable socket interrupts.
     *
     * \return Nothing if disabling socket interrupts succeeded.
     * \return An error code if disabling socket interrupts failed.
     */
    auto disable_socket_interrupts() noexcept
    {
        return m_driver->write_simr( 0x00 );
    }

    /**
     * \brief Get a mask identifying the sockets for which interrupts are enabled (SIMR
     *        register value).
     *
     * \return A mask identifying the sockets for which interrupts are enabled if getting
     *         a mask identifying the sockets for which interrupts are enabled succeeded.
     * \return An error code if getting a mask identifying the sockets for which interrupt
     *         are enabled failed.
     */
    auto enabled_socket_interrupts() const noexcept
    {
        return m_driver->read_simr();
    }

    /**
     * \brief Get the socket interrupt context (SIR register value).
     *
     * \return The socket interrupt context if getting the socket interrupt context
     *         succeeded.
     * \return An error code if getting the socket interrupt context failed.
     */
    auto socket_interrupt_context() const noexcept
    {
        return m_driver->read_sir();
    }

    /**
     * \brief Execute periodic housekeeping actions.
     */
    auto service() noexcept -> Result<Void, Error_Code>
    {
        return {};
    }

    /**
     * \brief Enable TCP ephemeral port allocation.
     *
     * \param[in] min The lower bound of the ephemeral port range.
     * \param[in] max The upper bound of the ephemeral port range.
     *
     * \return Nothing if enabling TCP ephemeral port allocation succeeded.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if TCP ephemeral port allocation is
     *         already enabled.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if min is greater than max.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if min is the port number that
     *         is used to represent any port number (0).
     */
    auto enable_tcp_ephemeral_port_allocation( IP::TCP::Port min, IP::TCP::Port max ) noexcept
        -> Result<Void, Error_Code>
    {
        if ( m_tcp_ephemeral_port_allocation_enabled ) {
            return Generic_Error::LOGIC_ERROR;
        } // if

        if ( not( min <= max ) ) {
            return Generic_Error::INVALID_ARGUMENT;
        } // if

        if ( min.is_any() ) {
            return Generic_Error::INVALID_ARGUMENT;
        } // if

        m_tcp_ephemeral_port_allocation_enabled = true;
        m_tcp_ephemeral_port_min                = min;
        m_tcp_ephemeral_port_max                = max;

        return {};
    }

  private:
    /**
     * \brief The driver for the W5500 the network stack utilizes.
     */
    Driver * m_driver{};

    /**
     * \brief The error code to return when an operation fails due to the W5500 being
     *        nonresponsive.
     */
    Error_Code m_nonresponsive_device_error{};

    /**
     * \brief The number of available sockets.
     */
    std::uint8_t m_available_sockets{ 16 / 2 };

    /**
     * \brief The TCP ephemeral port allocation enable state.
     */
    bool m_tcp_ephemeral_port_allocation_enabled{};

    /**
     * \brief The lower bound of the TCP epheral port range.
     */
    IP::TCP::Port m_tcp_ephemeral_port_min{};

    /**
     * \brief The upper bound of the TCP epheral port range.
     */
    IP::TCP::Port m_tcp_ephemeral_port_max{};
};

} // namespace picolibrary::WIZnet::W5500

#endif // PICOLIBRARY_WIZNET_W5500_NETWORK_STACK_H
