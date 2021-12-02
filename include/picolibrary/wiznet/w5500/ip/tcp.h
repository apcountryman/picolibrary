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
 * \brief picolibrary::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_WIZNET_W5500_IP_TCP_H

#include <cstdint>

#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 TCP over IP facilities namespace.
 */
namespace picolibrary::WIZnet::W5500::IP::TCP {

/**
 * \brief WIZnet W5500 TCP over IP client socket.
 *
 * \tparam Driver The W5500 driver implementation. The default W5500 driver implementation
 *         should be used unless a mock W5500 driver implementation is being injected to
 *         support unit testing of this client socket.
 * \tparam Network_Stack The W5500 IP network stack implementation. The default W5500 IP
 *         network stack implementation should be used unless a mock W5500 IP network
 *         stack implementation is being injected to support unit testing of this client
 *         socket.
 */
template<typename Driver, typename Network_Stack>
class Client {
  public:
    /**
     * \brief The unsigned integral type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::uint16_t;

    /**
     * \brief Socket state.
     */
    enum class State : std::uint_fast8_t {
        UNINITIALIZED, ///< Uninitialized.
        INITIALIZED,   ///< Initialized.
        BOUND,         ///< Bound.
        CONNECTING,    ///< Connecting.
        CONNECTED,     ///< Connected.
    };

    /**
     * \brief Constructor.
     */
    constexpr Client() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver for the W5500 the socket is associated with.
     * \param[in] socket_id The socket's socket ID.
     * \param[in] network_stack The network stack the socket is associated with.
     */
    constexpr Client( Driver & driver, Socket_ID socket_id, Network_Stack & network_stack ) noexcept :
        m_state{ State::INITIALIZED },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack }
    {
    }

#ifdef PICOLIBRARY_ENABLE_UNIT_TESTING
    /**
     * \brief Constructor.
     *
     * \param[in] state The socket's initial state.
     * \param[in] driver The driver for the W5500 the socket is associated with.
     * \param[in] socket_id The socket's socket ID.
     * \param[in] network_stack The network stack the socket is associated with.
     * \param[in] is_transmitting The socket's initial data transmission in progress
     *            status.
     */
    constexpr Client( State state, Driver & driver, Socket_ID socket_id, Network_Stack & network_stack, bool is_transmitting = false ) noexcept
        :
        m_state{ state },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack },
        m_is_transmitting{ is_transmitting }
    {
    }
#endif // PICOLIBRARY_ENABLE_UNIT_TESTING

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Client( Client && source ) noexcept :
        m_state{ source.m_state },
        m_driver{ source.m_driver },
        m_socket_id{ source.m_socket_id },
        m_network_stack{ source.m_network_stack },
        m_is_transmitting{ source.m_is_transmitting }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_driver        = nullptr;
        source.m_network_stack = nullptr;
    }

    Client( Client const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Client() noexcept
    {
        if ( m_state != State::UNINITIALIZED ) {
            m_network_stack->deallocate_socket( m_socket_id );
        } // if
    }

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto & operator=( Client && expression ) noexcept
    {
        if ( &expression != this ) {
            if ( m_state != State::UNINITIALIZED ) {
                m_network_stack->deallocate_socket( m_socket_id );
            } // if

            m_state           = expression.m_state;
            m_driver          = expression.m_driver;
            m_socket_id       = expression.m_socket_id;
            m_network_stack   = expression.m_network_stack;
            m_is_transmitting = expression.m_is_transmitting;

            expression.m_state         = State::UNINITIALIZED;
            expression.m_driver        = nullptr;
            expression.m_network_stack = nullptr;
        } // if

        return *this;
    }

    auto operator=( Client const & ) = delete;

    /**
     * \brief Get the socket's state.
     *
     * \return The socket's state.
     */
    constexpr auto state() const noexcept
    {
        return m_state;
    }

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
     * \brief Get the socket's socket interrupt mask (mask to be used when checking the
     *        network stack's socket interrupt context).
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
        return m_driver->write_sn_imr( m_socket_id, 0x00 );
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
     * \brief Clear interrupts.
     *
     * \param[in] mask The mask identifying the interrupts to clear.
     *
     * \return Nothing if clearing interrupts succeeded.
     * \return An error code if clearing interrupts failed.
     */
    auto clear_interrupts( std::uint8_t mask = Socket_Interrupt::ALL ) noexcept
        -> Result<Void, Error_Code>
    {
        auto result = m_driver->write_sn_ir( m_socket_id, mask );
        if ( result.is_error() ) {
            return result.error();
        } // if

        if ( mask & Socket_Interrupt::DATA_SENT ) {
            m_is_transmitting = false;
        } // if

        return {};
    }

    /**
     * \brief Configure the socket's no delayed ACK usage.
     *
     * \param[in] no_delayed_ack_configuration The desired no delayed ACK configuration.
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
     * \return An error code if getting the socket's no delayed ACK configuration failed.
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
     * \return The socket's maximum segment size if getting the socket's maximum segment
     *         size succeeded.
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
     * \return Nothing if configuring the socket's IPv4 packet time to live field value
     *         succeeded.
     * \return An error code if configuring the socket's IPv4 packet time to live field
     *         value failed.
     */
    auto configure_time_to_live( std::uint8_t time_to_live ) noexcept
    {
        return m_driver->write_sn_ttl( m_socket_id, time_to_live );
    }

    /**
     * \brief Get the socket's IPv4 packet time to live field value.
     *
     * \param[in] time_to_live The desired IPv4 packet time to live field value.
     *
     * \return The socket's IPv4 packet time to live field value if getting the socket's
     *         IPv4 packet time to live field value succeeded.
     * \return An error code if getting the socket's IPv4 packet time to live field value
     *         failed.
     */
    auto time_to_live() const noexcept
    {
        return m_driver->read_sn_ttl( m_socket_id );
    }

    /**
     * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
     *        register value).
     *
     * \param[in] keepalive_period The desired keepalive packet transmission period.
     *
     * \return Nothing if configuring the socket's keepalive packet transmission period
     *         succeeded.
     * \return An error code if configuring the socket's keepalive packet transmission
     *         period failed.
     */
    auto configure_keepalive_period( std::uint8_t keepalive_period ) noexcept
    {
        return m_driver->write_sn_kpalvtr( m_socket_id, keepalive_period );
    }

    /**
     * \brief Get the socket's keepalive packet transmission period (SN_KPALVTR register
     *        value).
     *
     * \return The socket's keepalive packet transmission period if getting the socket's
     *         keepalive packet transmission period succeeded.
     * \return An error code if getting the socket's keepalive packet transmission period
     *         failed.
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
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is not in a state
     *         that allows it to be bound to a local endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket has already been
     *         bound to a local endpoint.
     * \return picolibrary::Generic_Error::ENDPOINT_IN_USE if endpoint is already in use.
     * \return picolibrary::Generic_Error::EPHEMERAL_PORTS_EXHAUSTED if an ephemeral port
     *         was requested and no ephemeral ports are available.
     * \return An error code if binding the socket to the local endpoint failed for any
     *         other reason.
     */
    auto bind( ::picolibrary::IP::TCP::Endpoint const & endpoint = ::picolibrary::IP::TCP::Endpoint{} ) noexcept
        -> Result<Void, Error_Code>
    {
        // #lizard forgives the length
        // #lizard forgives the complexity

        if ( m_state != State::INITIALIZED ) {
            return Generic_Error::LOGIC_ERROR;
        } // if

        switch ( endpoint.address().version() ) {
            case ::picolibrary::IP::Version::UNSPECIFIED: break;
            case ::picolibrary::IP::Version::_4: break;
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

        if ( endpoint.port().is_any()
             and not m_network_stack->tcp_ephemeral_port_allocation_enabled() ) {
            return Generic_Error::EPHEMERAL_PORTS_EXHAUSTED;
        } // if

        SN_PORT::Type used_ports[ SOCKETS ];

        auto const available_sockets = m_network_stack->available_sockets();

        for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
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

        auto const is_available = [ &used_ports, available_sockets ]( SN_PORT::Type port ) noexcept -> bool {
            for ( auto socket = std::uint_fast8_t{}; socket < available_sockets; ++socket ) {
                if ( used_ports[ socket ] == port ) {
                    return false;
                } // if
            }     // for

            return true;
        };

        SN_PORT::Type port;

        if ( endpoint.port().is_any() ) {
            auto const ephemeral_port_min = m_network_stack->tcp_ephemeral_port_min().as_unsigned_integer();
            auto const ephemeral_port_max = m_network_stack->tcp_ephemeral_port_max().as_unsigned_integer();

            port = 0;

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
            port = endpoint.port().as_unsigned_integer();

            if ( not is_available( port ) ) {
                return Generic_Error::ENDPOINT_IN_USE;
            } // if
        }     // else

        {
            auto result = m_driver->write_sn_port( m_socket_id, port );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_sn_cr(
                m_socket_id, static_cast<SN_CR::Type>( Command::OPEN ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        for ( ;; ) {
            auto result = m_driver->read_sn_cr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( not result.value() ) {
                break;
            } // if
        }     // for

        for ( ;; ) {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: break;
                case Socket_Status::OPENED_TCP: m_state = State::BOUND; return {};
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }     // for
    }

    /**
     * \brief Connect to a remote endpoint.
     *
     * \param[in] endpoint The remote endpoint to connect to.
     *
     * \return Nothing if connecting to the remote endpoint succeeded.
     * \return picolibrary::Generic_Error::INVALID_ARGUMENT if endpoint is not a valid
     *         remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is not in a state
     *         that allows it to connect to a remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket is already connected
     *         to a remote endpoint.
     * \return picolibrary::Generic_Error::LOGIC_ERROR if the socket has not been bound to
     *         a local endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if connecting to the remote
     *         endpoint cannot succeed immediately.
     * \return picolibrary::Generic_Error::OPERATION_TIMEOUT if connecting to the remote
     *         endpoint timed out.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if connecting to the remote endpoint failed for any other
     *         reason.
     */
    auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint ) noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length
        // #lizard forgives the complexity

        if ( m_state == State::BOUND ) {
            if ( not endpoint.address().is_ipv4() or endpoint.address().ipv4().is_any()
                 or endpoint.port().is_any() ) {
                return Generic_Error::INVALID_ARGUMENT;
            } // if

            {
                auto result = m_driver->write_sn_dipr(
                    m_socket_id, endpoint.address().ipv4().as_byte_array() );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            {
                auto result = m_driver->write_sn_dport(
                    m_socket_id, endpoint.port().as_unsigned_integer() );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            {
                auto result = m_driver->write_sn_cr(
                    m_socket_id, static_cast<SN_CR::Type>( Command::CONNECT ) );
                if ( result.is_error() ) {
                    return result.error();
                } // if
            }

            for ( ;; ) {
                auto result = m_driver->read_sn_cr( m_socket_id );
                if ( result.is_error() ) {
                    return result.error();
                } // if

                if ( not result.value() ) {
                    m_state = State::CONNECTING;

                    return Generic_Error::WOULD_BLOCK;
                } // if
            }     // for
        }         // if

        if ( m_state == State::CONNECTING ) {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: return Generic_Error::OPERATION_TIMEOUT;
                case Socket_Status::ESTABLISHED: [[fallthrough]];
                case Socket_Status::CLOSE_WAIT: m_state = State::CONNECTED; return {};
                case Socket_Status::OPENED_TCP: [[fallthrough]];
                case Socket_Status::SYN_SENT: return Generic_Error::WOULD_BLOCK;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }     // if

        return Generic_Error::LOGIC_ERROR;
    }

    /**
     * \brief Check if the socket is connected to a remote endpoint.
     *
     * \return true if getting the socket's connection state succeeded and the socket is
     *         connected to a remote endpoint.
     * \return false if getting the socket's connection state succeeded and the socket is
     *         not connected to a remote endpoint.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if getting the socket's connection state failed for any other
     *         reason.
     */
    auto is_connected() const noexcept -> Result<bool, Error_Code>
    {
        auto result = m_driver->read_sn_sr( m_socket_id );
        if ( result.is_error() ) {
            return result.error();
        } // if

        switch ( static_cast<Socket_Status>( result.value() ) ) {
            case Socket_Status::CLOSED: return false;
            case Socket_Status::OPENED_TCP: return false;
            case Socket_Status::LISTEN: return false;
            case Socket_Status::ESTABLISHED: return true;
            case Socket_Status::CLOSE_WAIT: return false;
            case Socket_Status::SYN_SENT: return false;
            case Socket_Status::SYN_RECEIVED: return false;
            case Socket_Status::FIN_WAIT: return false;
            case Socket_Status::CLOSING: return false;
            case Socket_Status::TIME_WAIT: return false;
            case Socket_Status::LAST_ACK: return false;
            default: return m_network_stack->nonresponsive_device_error();
        } // switch
    }

    /**
     * \brief Get the connection's remote endpoint.
     *
     * \return The connection's remote endpoint if getting the connection's remote
     *         endpoint succeeded.
     * \return An error code if getting the connection's remote endpoint failed.
     */
    auto remote_endpoint() const noexcept -> Result<::picolibrary::IP::TCP::Endpoint, Error_Code>
    {
        SN_DIPR::Type sn_dipr;
        {
            auto result = m_driver->read_sn_dipr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            sn_dipr = result.value();
        }

        SN_DPORT::Type sn_dport;
        {
            auto result = m_driver->read_sn_dport( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            sn_dport = result.value();
        }

        return ::picolibrary::IP::TCP::Endpoint{ IPv4::Address{ sn_dipr }, sn_dport };
    }

    /**
     * \brief Get the connection's local endpoint.
     *
     * \return The connection's local endpoint if getting the connection's local endpoint
     *         succeeded.
     * \return An error code if getting the connection's local endpoint failed.
     */
    auto local_endpoint() const noexcept -> Result<::picolibrary::IP::TCP::Endpoint, Error_Code>
    {
        SIPR::Type sipr;
        {
            auto result = m_driver->read_sipr();
            if ( result.is_error() ) {
                return result.error();
            } // if

            sipr = result.value();
        }

        SN_PORT::Type sn_port;
        {
            auto result = m_driver->read_sn_port( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            sn_port = result.value();
        }

        return ::picolibrary::IP::TCP::Endpoint{ IPv4::Address{ sipr }, sn_port };
    }

    /**
     * \brief Get the amount of data that has yet to be transmitted to the remote
     *        endpoint.
     *
     * \return The amount of data that has yet to be transmitted to the remote endpoint if
     *         getting the amount of data that has yet to be transmitted to the remote
     *         endpoint succeeded.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if getting the amount of data that has yet to be transmitted
     *         to the remote endpoint failed for any other reason.
     */
    auto outstanding() const noexcept -> Result<Size, Error_Code>
    {
        // #lizard forgives the length

        Size buffer_size;
        {
            auto result = m_driver->read_sn_txbuf_size( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Buffer_Size>( result.value() ) ) {
                case Buffer_Size::_1_KIB: buffer_size = 1 * 1024; break;
                case Buffer_Size::_2_KIB: buffer_size = 2 * 1024; break;
                case Buffer_Size::_4_KIB: buffer_size = 4 * 1024; break;
                case Buffer_Size::_8_KIB: buffer_size = 8 * 1024; break;
                case Buffer_Size::_16_KIB: buffer_size = 16 * 1024; break;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        Size free_size;
        {
            auto result = m_driver->read_sn_tx_fsr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            free_size = result.value();
        }

        if ( free_size > buffer_size ) {
            return m_network_stack->nonresponsive_device_error();
        } // if

        return static_cast<Size>( buffer_size - free_size );
    }

    /**
     * \brief Transmit data to the remote endpoint.
     *
     * \param[in] begin The beginning of the block of data to write to the socket's
     *            transmit buffer.
     * \param[in] end The end of the block of data to write to the socket's transmit
     *            buffer.
     *
     * \return The end of the data that was written to the socket's transmit buffer if
     *         writing data to the socket's transmit buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if no data could be written to the
     *         socket's transmit buffer without blocking.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if writing data to the socket's transmit buffer failed for
     *         any other reason.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end ) noexcept
        -> Result<std::uint8_t const *, Error_Code>
    {
        // #lizard forgives the length
        // #lizard forgives the complexity

        if ( m_state != State::CONNECTED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::ESTABLISHED: break;
                case Socket_Status::FIN_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::CLOSE_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::CLOSING: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::TIME_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::LAST_ACK: return Generic_Error::NOT_CONNECTED;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        if ( m_is_transmitting ) {
            {
                {
                    auto result = m_driver->read_sn_ir( m_socket_id );
                    if ( result.is_error() ) {
                        return result.error();
                    } // if

                    if ( not( result.value() & Socket_Interrupt::DATA_SENT ) ) {
                        return Generic_Error::WOULD_BLOCK;
                    } // if
                }

                {
                    auto result = m_driver->write_sn_ir( m_socket_id, Socket_Interrupt::DATA_SENT );
                    if ( result.is_error() ) {
                        return result.error();
                    } // if
                }

                m_is_transmitting = false;
            }
        } // if

        if ( begin == end ) {
            return end;
        } // if

        {
            Size buffer_size;
            {
                auto result = m_driver->read_sn_txbuf_size( m_socket_id );
                if ( result.is_error() ) {
                    return result.error();
                } // if

                switch ( static_cast<Buffer_Size>( result.value() ) ) {
                    case Buffer_Size::_1_KIB: buffer_size = 1 * 1024; break;
                    case Buffer_Size::_2_KIB: buffer_size = 2 * 1024; break;
                    case Buffer_Size::_4_KIB: buffer_size = 4 * 1024; break;
                    case Buffer_Size::_8_KIB: buffer_size = 8 * 1024; break;
                    case Buffer_Size::_16_KIB: buffer_size = 16 * 1024; break;
                    default: return m_network_stack->nonresponsive_device_error();
                } // switch
            }

            Size free_size;
            {
                auto result = m_driver->read_sn_tx_fsr( m_socket_id );
                if ( result.is_error() ) {
                    return result.error();
                } // if

                free_size = result.value();
            }

            if ( free_size > buffer_size ) {
                return m_network_stack->nonresponsive_device_error();
            } // if

            if ( free_size == 0 ) {
                return Generic_Error::WOULD_BLOCK;
            } // if

            if ( end - begin > free_size ) {
                end = begin + free_size;
            } // if
        }

        SN_TX_WR::Type sn_tx_wr;
        {
            auto result = m_driver->read_sn_tx_wr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            sn_tx_wr = result.value();
        }

        {
            auto result = m_driver->write( m_socket_id, sn_tx_wr, begin, end );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_sn_tx_wr( m_socket_id, sn_tx_wr + ( end - begin ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_sn_cr(
                m_socket_id, static_cast<SN_CR::Type>( Command::SEND ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        for ( ;; ) {
            auto result = m_driver->read_sn_cr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( not result.value() ) {
                m_is_transmitting = true;

                return end;
            } // if
        }     // for
    }

    /**
     * \brief Check if data transmission is in progress.
     *
     * \return true if data transmission is in progress.
     * \return false if data transmission is not in progress.
     */
    constexpr auto is_transmitting() const noexcept
    {
        return m_is_transmitting;
    }

    /**
     * \brief Manually transmit a keepalive packet.
     *
     * \return Nothing if transmitting the keepalive packet succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device() if
     *         the W5500 is nonresponsive.
     * \return An error code if transmitting the keepalive packet failed for any other
     *         reason.
     */
    auto transmit_keepalive() noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        if ( m_state != State::CONNECTED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::ESTABLISHED: break;
                case Socket_Status::FIN_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::CLOSE_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::CLOSING: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::TIME_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::LAST_ACK: return Generic_Error::NOT_CONNECTED;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        {
            auto result = m_driver->write_sn_cr(
                m_socket_id, static_cast<SN_CR::Type>( Command::SEND_TCP_KEEPALIVE_PACKET ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        for ( ;; ) {
            auto result = m_driver->read_sn_cr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( not result.value() ) {
                return {};
            } // if
        }     // for
    }

    /**
     * \brief Get the amount of data that is immediately available to be received from the
     *        remote endpoint.
     *
     * \return The amount of data that is immediately available to be received from the
     *         remote endpoint if getting the amount of data that is immediately available
     *         to be received from the remote endpoint succeeded.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if getting the amount of data that is immediately available
     *         to be received from the remote endpoint failed for any other reason.
     */
    auto available() const noexcept -> Result<Size, Error_Code>
    {
        // #lizard forgives the length

        Size buffer_size;
        {
            auto result = m_driver->read_sn_rxbuf_size( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Buffer_Size>( result.value() ) ) {
                case Buffer_Size::_1_KIB: buffer_size = 1 * 1024; break;
                case Buffer_Size::_2_KIB: buffer_size = 2 * 1024; break;
                case Buffer_Size::_4_KIB: buffer_size = 4 * 1024; break;
                case Buffer_Size::_8_KIB: buffer_size = 8 * 1024; break;
                case Buffer_Size::_16_KIB: buffer_size = 16 * 1024; break;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        Size received_size;
        {
            auto result = m_driver->read_sn_rx_rsr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            received_size = result.value();
        }

        if ( received_size > buffer_size ) {
            return m_network_stack->nonresponsive_device_error();
        } // if

        return received_size;
    }

    /**
     * \brief Receive data from the remote endpoint.
     *
     * \param[out] begin The beginning of the block of data read from the socket's receive
     *             buffer.
     * \param[out] end The end of the block of data read from the socket's receive buffer.
     *
     * \return The end of the data that was read from the socket's receive buffer if
     *         reading data from the socket's receive buffer succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::Generic_Error::WOULD_BLOCK if no data could be read from the
     *         socket's receive buffer without blocking.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if reading data from the socket's receive buffer failed for
     *         any other reason.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) noexcept
        -> Result<std::uint8_t *, Error_Code>
    {
        // #lizard forgives the length
        // #lizard forgives the complexity

        if ( m_state != State::CONNECTED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        auto close_wait = false;
        {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::ESTABLISHED: break;
                case Socket_Status::FIN_WAIT: return Generic_Error::WOULD_BLOCK;
                case Socket_Status::CLOSE_WAIT: close_wait = true; break;
                case Socket_Status::CLOSING: return Generic_Error::WOULD_BLOCK;
                case Socket_Status::TIME_WAIT: return Generic_Error::WOULD_BLOCK;
                case Socket_Status::LAST_ACK: return Generic_Error::WOULD_BLOCK;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        {
            Size buffer_size;
            {
                auto result = m_driver->read_sn_rxbuf_size( m_socket_id );
                if ( result.is_error() ) {
                    return result.error();
                } // if

                switch ( static_cast<Buffer_Size>( result.value() ) ) {
                    case Buffer_Size::_1_KIB: buffer_size = 1 * 1024; break;
                    case Buffer_Size::_2_KIB: buffer_size = 2 * 1024; break;
                    case Buffer_Size::_4_KIB: buffer_size = 4 * 1024; break;
                    case Buffer_Size::_8_KIB: buffer_size = 8 * 1024; break;
                    case Buffer_Size::_16_KIB: buffer_size = 16 * 1024; break;
                    default: return m_network_stack->nonresponsive_device_error();
                } // switch
            }

            Size received_size;
            {
                auto result = m_driver->read_sn_rx_rsr( m_socket_id );
                if ( result.is_error() ) {
                    return result.error();
                } // if

                received_size = result.value();
            }

            if ( received_size > buffer_size ) {
                return m_network_stack->nonresponsive_device_error();
            } // if

            if ( received_size == 0 ) {
                if ( close_wait ) {
                    return Generic_Error::NOT_CONNECTED;
                } else {
                    return Generic_Error::WOULD_BLOCK;
                } // else
            }     // if

            if ( begin == end ) {
                return end;
            } // if

            if ( end - begin > received_size ) {
                end = begin + received_size;
            } // if
        }

        SN_RX_RD::Type sn_rx_rd;
        {
            auto result = m_driver->read_sn_rx_rd( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            sn_rx_rd = result.value();
        }

        {
            auto result = m_driver->read( m_socket_id, sn_rx_rd, begin, end );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_sn_rx_rd( m_socket_id, sn_rx_rd + ( end - begin ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        {
            auto result = m_driver->write_sn_cr(
                m_socket_id, static_cast<SN_CR::Type>( Command::RECEIVE ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        for ( ;; ) {
            auto result = m_driver->read_sn_cr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( not result.value() ) {
                return end;
            } // if
        }     // for
    }

    /**
     * \brief Disable further data transmission and reception.
     *
     * \return Nothing if disabling further data transmission and reception succeeded.
     * \return picolibrary::Generic_Error::NOT_CONNECTED if the socket is not connected to
     *         a remote endpoint.
     * \return picolibrary::WIZnet::W5500::IP::Network_Stack::nonresponsive_device_error()
     *         if the W5500 is nonresponsive.
     * \return An error code if disabling further data transmission and reception failed
     *         for any other reason.
     */
    auto shutdown() noexcept -> Result<Void, Error_Code>
    {
        // #lizard forgives the length

        if ( m_state != State::CONNECTED ) {
            return Generic_Error::NOT_CONNECTED;
        } // if

        {
            auto result = m_driver->read_sn_sr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            switch ( static_cast<Socket_Status>( result.value() ) ) {
                case Socket_Status::CLOSED: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::ESTABLISHED: break;
                case Socket_Status::FIN_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::CLOSE_WAIT: break;
                case Socket_Status::CLOSING: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::TIME_WAIT: return Generic_Error::NOT_CONNECTED;
                case Socket_Status::LAST_ACK: return Generic_Error::NOT_CONNECTED;
                default: return m_network_stack->nonresponsive_device_error();
            } // switch
        }

        {
            auto result = m_driver->write_sn_cr(
                m_socket_id, static_cast<SN_CR::Type>( Command::DISCONNECT ) );
            if ( result.is_error() ) {
                return result.error();
            } // if
        }

        for ( ;; ) {
            auto result = m_driver->read_sn_cr( m_socket_id );
            if ( result.is_error() ) {
                return result.error();
            } // if

            if ( not result.value() ) {
                return {};
            } // if
        }     // for
    }

    /**
     * \brief Close the socket.
     *
     * \return Success.
     */
    auto close() noexcept -> Result<Void, Void>
    {
        if ( m_state != State::UNINITIALIZED ) {
            m_network_stack->deallocate_socket( m_socket_id );

            m_state = State::UNINITIALIZED;
        } // if

        return {};
    }

  private:
    /**
     * \brief The socket's state.
     */
    State m_state{ State::UNINITIALIZED };

    /**
     * \brief The driver for the W5500 the socket is associated with.
     */
    Driver * m_driver{};

    /**
     * \brief The socket's socket ID.
     */
    Socket_ID m_socket_id{};

    /**
     * \brief The network stack the socket is associated with.
     */
    Network_Stack * m_network_stack{};

    /**
     * \brief The socket's data transmission in progress status.
     */
    bool m_is_transmitting{};
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H
