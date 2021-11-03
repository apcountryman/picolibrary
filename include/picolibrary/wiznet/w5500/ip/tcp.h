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
 *         support unit testing of this network stack.
 * \tparam Network_Stack The W5500 IP network stack implementation.
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
     *
     * \attention This type is only exposed to support unit testing.
     */
    enum class State : std::uint_fast8_t {
        UNINITIALIZED, ///< Uninitialized.
        INITIALIZED,   ///< Initialized.
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
     */
    constexpr Client( State state, Driver & driver, Socket_ID socket_id, Network_Stack & network_stack ) noexcept
        :
        m_state{ state },
        m_driver{ &driver },
        m_socket_id{ socket_id },
        m_network_stack{ &network_stack }
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
        m_network_stack{ source.m_network_stack }
    {
        source.m_state         = State::UNINITIALIZED;
        source.m_driver        = nullptr;
        source.m_network_stack = nullptr;
    }

    Client( Client const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Client() noexcept = default;

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
            m_state         = expression.m_state;
            m_driver        = expression.m_driver;
            m_socket_id     = expression.m_socket_id;
            m_network_stack = expression.m_network_stack;

            expression.m_state         = State::UNINITIALIZED;
            expression.m_driver        = nullptr;
            expression.m_network_stack = nullptr;
        } // if

        return *this;
    }

    auto operator=( Client const & ) = delete;

#ifdef PICOLIBRARY_ENABLE_UNIT_TESTING
    /**
     * \brief Get the socket's state.
     *
     * \return The socket's state.
     */
    constexpr auto state() const noexcept
    {
        return m_state;
    }
#endif // PICOLIBRARY_ENABLE_UNIT_TESTING

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
     *
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
};

} // namespace picolibrary::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_WIZNET_W5500_IP_TCP_H
