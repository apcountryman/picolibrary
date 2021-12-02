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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H

#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 TCP over IP unit testing facilities.
 */
namespace picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP {

/**
 * \brief Mock WIZnet W5500 IP TCP client socket.
 */
class Mock_Client {
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
     * \brief Movable mock client socket.
     */
    class Handle {
      public:
        /**
         * \brief The unsigned integral type used to report transmit/receive buffer
         *        information.
         */
        using Size = std::uint16_t;

        /**
         * \brief Socket state.
         */
        using State = ::picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP::Mock_Client::State;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_client The mock client socket.
         */
        Handle( Mock_Client & mock_client ) noexcept : m_mock_client{ &mock_client }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept : m_mock_client{ source.m_mock_client }
        {
            source.m_mock_client = nullptr;
        }

        Handle( Handle const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Handle() noexcept = default;

        /**
         * \brief Assignment operator.
         *
         * \param[in] expression The expression to be assigned.
         *
         * \return The assigned to object.
         */
        auto & operator=( Handle && expression ) noexcept
        {
            if ( &expression != this ) {
                m_mock_client = expression.m_mock_client;

                expression.m_mock_client = nullptr;
            } // if

            return *this;
        }

        auto operator=( Handle const & ) = delete;

        /**
         * \brief Get the mock client socket.
         *
         * \return The mock client socket.
         */
        auto & mock() noexcept
        {
            return *m_mock_client;
        }

        /**
         * \brief Get the socket's state.
         *
         * \return The socket's state.
         */
        auto state() const
        {
            return m_mock_client->state();
        }

        /**
         * \brief Get the socket's socket ID.
         *
         * \return The socket's socket ID.
         */
        auto socket_id() const
        {
            return m_mock_client->socket_id();
        }

        /**
         * \brief Get the socket's socket interrupt mask (mask to be used when checking
         *        the network stack's socket interrupt context).
         *
         * \return The socket's socket interrupt mask.
         */
        auto socket_interrupt_mask() const
        {
            return m_mock_client->socket_interrupt_mask();
        }

        /**
         * \brief Enable interrupts.
         *
         * \param[in] mask The mask identifying the interrupts to enable.
         *
         * \return Nothing if enabling interrupts succeeded.
         * \return An error code if enabling interrupts failed.
         */
        auto enable_interrupts( std::uint8_t mask )
        {
            return m_mock_client->enable_interrupts( mask );
        }

        /**
         * \brief Disable interrupts.
         *
         * \param[in] mask The mask identifying the interrupts to disable.
         *
         * \return Nothing if disabling interrupts succeeded.
         * \return An error code if disabling interrupts failed.
         */
        auto disable_interrupts( std::uint8_t mask )
        {
            return m_mock_client->disable_interrupts( mask );
        }

        /**
         * \brief Disable all interrupts.
         *
         * \return Nothing if disabling all interrupts succeeded.
         * \return An error code if disabling all interrupts failed.
         */
        auto disable_interrupts()
        {
            return m_mock_client->disable_interrupts();
        }

        /**
         * \brief Get a mask identifying the interrupts that are enabled.
         *
         * \return A mask identifying the interrupts that are enabled if getting a mask
         *         identifying the interrupts that are enabled succeeded.
         * \return An error code if getting a mask identifying the interrupts that are
         *         enabled failed.
         */
        auto enabled_interrupts() const
        {
            return m_mock_client->enabled_interrupts();
        }

        /**
         * \brief Get the interrupt context (SN_IR register value).
         *
         * \return The interrupt context if getting the interrupt context succeeded.
         * \return An error code if getting the interrupt context failed.
         */
        auto interrupt_context() const
        {
            return m_mock_client->interrupt_context();
        }

        /**
         * \brief Clear interrupts.
         *
         * \return Nothing if clearing interrupts succeeded.
         * \return An error code if clearing interrupts failed.
         */
        auto clear_interrupts()
        {
            return m_mock_client->clear_interrupts();
        }

        /**
         * \brief Clear interrupts.
         *
         * \param[in] mask The mask identifying the interrupts to clear.
         *
         * \return Nothing if clearing interrupts succeeded.
         * \return An error code if clearing interrupts failed.
         */
        auto clear_interrupts( std::uint8_t mask )
        {
            return m_mock_client->clear_interrupts( mask );
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
        auto configure_no_delayed_ack( ::picolibrary::WIZnet::W5500::No_Delayed_ACK no_delayed_ack_configuration )
        {
            return m_mock_client->configure_no_delayed_ack( no_delayed_ack_configuration );
        }

        /**
         * \brief Get the socket's no delayed ACK configuration.
         *
         * \return The socket's no delayed ACK configuration if getting the socket's no
         *         delayed ACK configuration succeeded.
         * \return An error code if getting the socket's no delayed ACK configuration
         *         failed.
         */
        auto no_delayed_ack_configuration() const
        {
            return m_mock_client->no_delayed_ack_configuration();
        }

        /**
         * \brief Configure the socket's maximum segment size.
         *
         * \param[in] maximum_segment_size The desired maximum segment size.
         *
         * \return Nothing if configuring the socket's maximum segment size succeeded.
         * \return An error code if configuring the socket's maximum segment size failed.
         */
        auto configure_maximum_segment_size( std::uint16_t maximum_segment_size )
        {
            return m_mock_client->configure_maximum_segment_size( maximum_segment_size );
        }

        /**
         * \brief Get the socket's maximum segment size.
         *
         * \return The socket's maximum segment size if getting the socket's maximum
         *         segment size succeeded.
         * \return An error code if getting the socket's maximum segment size failed.
         */
        auto maximum_segment_size() const
        {
            return m_mock_client->maximum_segment_size();
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
        auto configure_time_to_live( std::uint8_t time_to_live )
        {
            return m_mock_client->configure_time_to_live( time_to_live );
        }

        /**
         * \brief Get the socket's IPv4 packet time to live field value.
         *
         * \return The socket's IPv4 packet time to live field value if getting the
         *         socket's IPv4 packet time to live field value succeeded.
         * \return An error code if getting the socket's IPv4 packet time to live field
         *         value failed.
         */
        auto time_to_live() const
        {
            return m_mock_client->time_to_live();
        }

        /**
         * \brief Configure the socket's keepalive packet transmission period (SN_KPALVTR
         *        register value).
         *
         * \param[in] keepalive_period The desired keepalive packet transmission period.
         *
         * \return Nothing if configuring the socket's keepalive packet transmission
         *         period succeeded.
         * \return An error code if configuring the socket's keepalive packet transmission
         *         period failed.
         */
        auto configure_keepalive_period( std::uint8_t keepalive_period )
        {
            return m_mock_client->configure_keepalive_period( keepalive_period );
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
        auto keepalive_period() const
        {
            return m_mock_client->keepalive_period();
        }

        /**
         * \brief Bind the socket to a specific local endpoint.
         *
         * \return Nothing if binding the socket to the local endpoint succeeded.
         * \return An error code if binding the socket to the local endpoint failed.
         */
        auto bind()
        {
            return m_mock_client->bind();
        }

        /**
         * \brief Bind the socket to a specific local endpoint.
         *
         * \param[in] endpoint The local endpoint to bind the socket to.
         *
         * \return Nothing if binding the socket to the local endpoint succeeded.
         * \return An error code if binding the socket to the local endpoint failed.
         */
        auto bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return m_mock_client->bind( endpoint );
        }

        /**
         * \brief Connect to a remote endpoint.
         *
         * \param[in] endpoint The remote endpoint to connect to.
         *
         * \return Nothing if connecting to a remote endpoint succeeded.
         * \return An error code if connecting to the remote endpoint failed.
         */
        auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return m_mock_client->connect( endpoint );
        }

        /**
         * \brief Check if the socket is connected to a remote endpoint.
         *
         * \return true if getting the socket's connection state succeeded and the socket
         *         is connected to a remote endpoint.
         * \return false if getting the socket's connection state succeeded and the socket
         *         is not connected to a remote endpoint.
         * \return An error code if getting the socket's connection state failed.
         */
        auto is_connected() const
        {
            return m_mock_client->is_connected();
        }

        /**
         * \brief Get the connection's remote endpoint.
         *
         * \return The connection's remote endpoint if getting the connection's remote
         *         endpoint succeeded.
         * \return An error code if getting the connection's remote endpoint failed.
         */
        auto remote_endpoint() const
        {
            return m_mock_client->remote_endpoint();
        }

        /**
         * \brief Get the connection's local endpoint.
         *
         * \return The connection's local endpoint if getting the connection's local
         *         endpoint succeeded.
         * \return An error code if getting the connection's local endpoint failed.
         */
        auto local_endpoint() const
        {
            return m_mock_client->local_endpoint();
        }

        /**
         * \brief Get the amount of data that has yet to be transmitted to the remote
         *        endpoint.
         *
         * \return The amount of data that has yet to be transmitted to the remote
         *         endpoint if getting the amount of data that has yet to be transmitted
         *         to the remote endpoint succeeded.
         * \return An error code if getting the amount of data that has yet to be
         *         transmitted to the remote endpoint failed.
         */
        auto outstanding() const
        {
            return m_mock_client->outstanding();
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
         * \return An error code if writing data to the socket's transmit buffer failed.
         */
        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return m_mock_client->transmit( begin, end );
        }

        /**
         * \brief Check if data transmission is in progress.
         *
         * \return true if data transmission is in progress.
         * \return false if data transmission is not in progress.
         */
        auto is_transmitting() const
        {
            return m_mock_client->is_transmitting();
        }

        /**
         * \brief Manually transmit a keepalive packet.
         *
         * \return Nothing if transmitting the keepalive packet succeeded.
         * \return An error code if transmitting the keepalive packet failed.
         */
        auto transmit_keepalive()
        {
            return m_mock_client->transmit_keepalive();
        }

        /**
         * \brief Get the amount of data that is immediately available to be received from
         *        the remote endpoint.
         *
         * \return The amount of data that is immediately available to be received from
         *         the remote endpoint if getting the amount of data that is immediately
         *         available to be received from the remote endpoint succeeded.
         * \return An error code if getting the amount of data that is immediately
         *         available to be received from the remote endpoint failed.
         */
        auto available() const
        {
            return m_mock_client->available();
        }

        /**
         * \brief Receive data from the remote endpoint.
         *
         * \param[out] begin The beginning of the block of data read from the socket's
         *             receive buffer.
         * \param[out] end The end of the block of data read from the socket's receive
         *             buffer.
         *
         * \return The end of the data that was read from the socket's receive buffer if
         *         reading data from the socket's receive buffer succeeded.
         * \return An error code if reading data from the socket's receive buffer failed.
         */
        auto receive( std::uint8_t * begin, std::uint8_t * end )
        {
            return m_mock_client->receive( begin, end );
        }

        /**
         * \brief Disable further data transmission and reception.
         *
         * \return Nothing if disabling further data transmission and reception succeeded.
         * \return An error code if disabling further data transmission and reception
         *         failed.
         */
        auto shutdown()
        {
            return m_mock_client->shutdown();
        }

        /**
         * \brief Close the socket.
         *
         * \return Success.
         */
        auto close()
        {
            return m_mock_client->close();
        }

      private:
        /**
         * \brief The mock client socket.
         */
        Mock_Client * m_mock_client{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Client() = default;

    Mock_Client( Mock_Client && ) = delete;

    Mock_Client( Mock_Client const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Client() noexcept = default;

    auto operator=( Mock_Client && ) = delete;

    auto operator=( Mock_Client const & ) = delete;

    /**
     * \brief Get a movable handle to the mock client.
     *
     * \return A movable handle to the mock client.
     */
    auto handle()
    {
        return Handle{ *this };
    }

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );
    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), enable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, ( std::uint8_t ) );
    MOCK_METHOD( (Result<Void, Error_Code>), disable_interrupts, () );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), enabled_interrupts, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), interrupt_context, (), ( const ) );
    MOCK_METHOD( (Result<Void, Error_Code>), clear_interrupts, () );
    MOCK_METHOD( (Result<Void, Error_Code>), clear_interrupts, ( std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_no_delayed_ack, ( ::picolibrary::WIZnet::W5500::No_Delayed_ACK ) );
    MOCK_METHOD(
        (Result<::picolibrary::WIZnet::W5500::No_Delayed_ACK, Error_Code>),
        no_delayed_ack_configuration,
        (),
        ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_maximum_segment_size, ( std::uint16_t ) );
    MOCK_METHOD( (Result<std::uint16_t, Error_Code>), maximum_segment_size, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_time_to_live, ( std::uint8_t ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), time_to_live, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), configure_keepalive_period, ( std::uint8_t ) );
    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), keepalive_period, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );
    MOCK_METHOD( (Result<Void, Error_Code>), bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), remote_endpoint, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Size, Error_Code>), outstanding, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

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
     * \return An error code if writing data to the socket's transmit buffer failed.
     */
    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( bool, is_transmitting, (), ( const ) );

    MOCK_METHOD( (Result<Void, Error_Code>), transmit_keepalive, () );

    MOCK_METHOD( (Result<Size, Error_Code>), available, (), ( const ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

    /**
     * \brief Receive data from the remote endpoint.
     *
     * \param[out] begin The beginning of the block of data read from the socket's receive
     *             buffer.
     * \param[out] end The end of the block of data read from the socket's receive buffer.
     *
     * \return The end of the data that was read from the socket's receive buffer if
     *         reading data from the socket's receive buffer succeeded.
     * \return An error code if reading data from the socket's receive buffer failed.
     */
    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return begin;
    }

    MOCK_METHOD( (Result<Void, Error_Code>), shutdown, () );

    MOCK_METHOD( (Result<Void, Void>), close, () );
};

} // namespace picolibrary::Testing::Unit::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_IP_TCP_H
