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
 * \brief picolibrary::Testing::Unit::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_IP_TCP_H
#define PICOLIBRARY_TESTING_UNIT_IP_TCP_H

#include <cstddef>
#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/ip.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/void.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random TCP over IP port number.
 *
 * \return A pseudo-random TCP over IP port number.
 */
template<>
inline auto random<::picolibrary::IP::TCP::Port>()
{
    return ::picolibrary::IP::TCP::Port{ random<::picolibrary::IP::TCP::Port::Unsigned_Integer>() };
}

/**
 * \brief Generate a pseudo-random TCP over IP endpoint.
 *
 * \brief A pseudo-random TCP over IP endpoint.
 */
template<>
inline auto random<::picolibrary::IP::TCP::Endpoint>()
{
    return ::picolibrary::IP::TCP::Endpoint{ random<::picolibrary::IP::Address>(),
                                             random<::picolibrary::IP::TCP::Port>() };
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Transmission Control Protocol (TCP) over IP unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IP::TCP {

/**
 * \brief Mock client socket.
 */
class Mock_Client {
  public:
    /**
     * \brief Endpoint.
     */
    using Endpoint = ::picolibrary::IP::TCP::Endpoint;

    /**
     * \brief The unsigned integral type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Movable mock client socket handle.
     */
    class Handle {
      public:
        /**
         * \brief The unsigned integral type used to report transmit/receive buffer
         *        information.
         */
        using Size = std::size_t;

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
         * \brief Bind the socket to a specific local endpoint.
         *
         * \param[in] endpoint The local endpoint to bind the socket to.
         *
         * \return Nothing if binding the socket to the local endpoint succeeded.
         * \return An error code if binding the socket to the local endpoint failed.
         */
        auto bind( Endpoint const & endpoint = Endpoint{} )
        {
            return m_mock_client->bind( endpoint );
        }

        /**
         * \brief Connect to a remote endpoint.
         *
         * \param[in] endpoint The remote endpoint to connect to.
         *
         * \return Nothing if connecting to the remote endpoint succeeded.
         * \return An error code if connecting to the remote endpoint failed.
         */
        auto connect( Endpoint const & endpoint )
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
         * \return Nothing if closing the socket succeeded.
         * \return An error code if closing the socket failed.
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

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );

    MOCK_METHOD( (Result<Void, Error_Code>), bind, (Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (Endpoint const &));

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );

    MOCK_METHOD( (Result<Endpoint, Error_Code>), remote_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Endpoint, Error_Code>), local_endpoint, (), ( const ) );

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

    MOCK_METHOD( (Result<Void, Error_Code>), close, () );
};

/**
 * \brief Mock server socket.
 */
class Mock_Server {
  public:
    /**
     * \brief Endpoint.
     */
    using Endpoint = ::picolibrary::IP::TCP::Endpoint;

    /**
     * \brief The unsigned integral type used to report transmit/receive buffer
     *        information.
     */
    using Size = std::size_t;

    /**
     * \brief Movable mock server socket handle.
     */
    class Handle {
      public:
        /**
         * \brief The unsigned integral type used to report transmit/receive buffer
         *        information.
         */
        using Size = std::size_t;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_server The mock server socket.
         */
        Handle( Mock_Server & mock_server ) noexcept : m_mock_server{ &mock_server }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept : m_mock_server{ source.m_mock_server }
        {
            source.m_mock_server = nullptr;
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
                m_mock_server = expression.m_mock_server;

                expression.m_mock_server = nullptr;
            } // if

            return *this;
        }

        /**
         * \brief Get the mock server socket.
         *
         * \return The mock server socket.
         */
        auto & mock() noexcept
        {
            return *m_mock_server;
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
            return m_mock_server->is_connected();
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
            return m_mock_server->remote_endpoint();
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
            return m_mock_server->local_endpoint();
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
            return m_mock_server->outstanding();
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
            return m_mock_server->transmit( begin, end );
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
            return m_mock_server->available();
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
            return m_mock_server->receive( begin, end );
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
            return m_mock_server->shutdown();
        }

        /**
         * \brief Close the socket.
         *
         * \return Nothing if closing the socket succeeded.
         * \return An error code if closing the socket failed.
         */
        auto close()
        {
            return m_mock_server->close();
        }

      private:
        /**
         * \brief The mock server socket.
         */
        Mock_Server * m_mock_server{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Server() = default;

    Mock_Server( Mock_Server && ) = delete;

    Mock_Server( Mock_Server const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Server() noexcept = default;

    auto operator=( Mock_Server && ) = delete;

    auto operator=( Mock_Server const & ) = delete;

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );

    MOCK_METHOD( (Result<Endpoint, Error_Code>), remote_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Endpoint, Error_Code>), local_endpoint, (), ( const ) );

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

    MOCK_METHOD( (Result<Void, Error_Code>), close, () );
};

/**
 * \brief Mock acceptor socket.
 */
class Mock_Acceptor {
  public:
    /**
     * \brief Endpoint.
     */
    using Endpoint = ::picolibrary::IP::TCP::Endpoint;

    /**
     * \brief The type of server socket produced by the acceptor socket.
     */
    using Server = Mock_Server::Handle;

    /**
     * \brief Movable mock acceptor socket handle.
     */
    class Handle {
      public:
        /**
         * \brief The type of server socket produced by the acceptor socket.
         */
        using Server = Mock_Server::Handle;

        /**
         * \brief Constructor.
         */
        Handle() noexcept = default;

        /**
         * \brief Constructor.
         *
         * \param[in] mock_acceptor The mock acceptor socket.
         */
        Handle( Mock_Acceptor & mock_acceptor ) noexcept :
            m_mock_acceptor{ &mock_acceptor }
        {
        }

        /**
         * \brief Constructor.
         *
         * \param[in] source The source of the move.
         */
        Handle( Handle && source ) noexcept : m_mock_acceptor{ source.m_mock_acceptor }
        {
            source.m_mock_acceptor = nullptr;
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
                m_mock_acceptor = expression.m_mock_acceptor;

                expression.m_mock_acceptor = nullptr;
            } // if

            return *this;
        }

        /**
         * \brief Get the mock acceptor socket.
         *
         * \return The mock acceptor socket.
         */
        auto & mock() noexcept
        {
            return *m_mock_acceptor;
        }

        /**
         * \brief Bind the socket to a specific local endpoint.
         *
         * \param[in] endpoint The local endpoint to bind the socket to.
         *
         * \return Nothing if binding the socket to the local endpoint succeeded.
         * \return An error code if binding the socket to the local endpoint failed.
         */
        auto bind( Endpoint const & endpoint = Endpoint{} )
        {
            return m_mock_acceptor->bind( endpoint );
        }

        /**
         * \brief Listen for incoming connection requests.
         *
         * \param[in] backlog The maximum number of simultaneously connected clients.
         *
         * \return Nothing if listening for incoming connection requests succeeded.
         * \return An error code if listening for incoming connection requests failed.
         */
        auto listen( std::uint_fast8_t backlog )
        {
            return m_mock_acceptor->listen( backlog );
        }

        /**
         * \brief Check if the socket is listening for incoming connection requests.
         *
         * \return true if getting the socket's listening state succeeded and the socket
         *         is listening for incoming connection requests.
         * \return false if getting the socket's listening state succeeded and the socket
         *         is not listening for incoming connection requests.
         * \return An error code if getting the socket's listening state failed.
         */
        auto is_listening() const
        {
            return m_mock_acceptor->is_listening();
        }

        /**
         * \brief Get the endpoint on which the socket is listening for incoming
         *        connection requests.
         *
         * \return The endpoint on which the socket is listening for incoming connection
         *         requests if getting the endpoint on which the socket is listening for
         *         incoming connection requests succeeded.
         * \return An error code if getting the endpoint on which the socket is listening
         *         for incoming connection requests failed.
         */
        auto local_endpoint() const
        {
            return m_mock_acceptor->local_endpoint();
        }

        /**
         * \brief Accept an incoming connection request.
         *
         * \return A server socket for handling the connection if accepting an incoming
         *         connection request succeeded.
         * \return An error code if accepting an incoming connection request failed.
         */
        auto accept()
        {
            return m_mock_acceptor->accept();
        }

        /**
         * \brief Close the socket.
         *
         * \return Nothing if closing the socket succeeded.
         * \return An error code if closing the socket failed.
         */
        auto close()
        {
            return m_mock_acceptor->close();
        }

      private:
        /**
         * \brief The mock acceptor socket.
         */
        Mock_Acceptor * m_mock_acceptor{};
    };

    /**
     * \brief Constructor.
     */
    Mock_Acceptor() = default;

    Mock_Acceptor( Mock_Acceptor && ) = delete;

    Mock_Acceptor( Mock_Acceptor const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Acceptor() noexcept = default;

    auto operator=( Mock_Acceptor && ) = delete;

    auto operator=( Mock_Acceptor const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );

    MOCK_METHOD( (Result<Void, Error_Code>), bind, (Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), listen, ( std::uint_fast8_t ) );

    MOCK_METHOD( (Result<bool, Error_Code>), is_listening, (), ( const ) );

    MOCK_METHOD( (Result<Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Server, Error_Code>), accept, () );

    MOCK_METHOD( (Result<Void, Error_Code>), close, () );
};

} // namespace picolibrary::Testing::Unit::IP::TCP

#endif // PICOLIBRARY_TESTING_UNIT_IP_TCP_H
