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
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/ip.h"
#include "picolibrary/testing/unit/mock_handle.h"
#include "picolibrary/void.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP unit testing facilities.
 */
namespace picolibrary::Testing::Unit::IP::TCP {

/**
 * \brief Mock client socket.
 */
class Mock_Client {
  public:
    using Size = std::size_t;

    class Handle : public Mock_Handle<Mock_Client> {
      public:
        using Size = Mock_Client::Size;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Client & mock ) noexcept : Mock_Handle<Mock_Client>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto bind()
        {
            return mock().bind();
        }

        auto bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return mock().bind( endpoint );
        }

        auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return mock().connect( endpoint );
        }

        auto is_connected() const
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const
        {
            return mock().local_endpoint();
        }

        auto outstanding() const
        {
            return mock().outstanding();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return mock().transmit( begin, end );
        }

        auto available() const
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
        {
            return mock().receive( begin, end );
        }

        auto shutdown()
        {
            return mock().shutdown();
        }

        auto close()
        {
            return mock().close();
        }
    };

    Mock_Client() = default;

    Mock_Client( Mock_Client && ) = delete;

    Mock_Client( Mock_Client const & ) = delete;

    ~Mock_Client() noexcept = default;

    auto operator=( Mock_Client && ) = delete;

    auto operator=( Mock_Client const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );
    MOCK_METHOD( (Result<Void, Error_Code>), bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );

    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), remote_endpoint, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Size, Error_Code>), outstanding, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<Size, Error_Code>), available, (), ( const ) );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

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
    using Size = std::size_t;

    class Handle : public Mock_Handle<Mock_Server> {
      public:
        using Size = Mock_Server::Size;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Server & mock ) noexcept : Mock_Handle<Mock_Server>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto is_connected() const
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const
        {
            return mock().local_endpoint();
        }

        auto outstanding() const
        {
            return mock().outstanding();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        {
            return mock().transmit( begin, end );
        }

        auto available() const
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
        {
            return mock().receive( begin, end );
        }

        auto shutdown()
        {
            return mock().shutdown();
        }

        auto close()
        {
            return mock().close();
        }
    };

    Mock_Server() = default;

    Mock_Server( Mock_Server && ) = delete;

    Mock_Server( Mock_Server const & ) = delete;

    ~Mock_Server() noexcept = default;

    auto operator=( Mock_Server && ) = delete;

    auto operator=( Mock_Server const & ) = delete;

    MOCK_METHOD( (Result<bool, Error_Code>), is_connected, (), ( const ) );

    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), remote_endpoint, (), ( const ) );
    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Size, Error_Code>), outstanding, (), ( const ) );
    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<Size, Error_Code>), available, (), ( const ) );
    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

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
    using Server = Mock_Server::Handle;

    class Handle : public Mock_Handle<Mock_Acceptor> {
      public:
        using Server = Mock_Acceptor::Server;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Acceptor & mock ) noexcept :
            Mock_Handle<Mock_Acceptor>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto bind()
        {
            return mock().bind();
        }

        auto bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            return mock().bind( endpoint );
        }

        auto listen( std::uint_fast8_t backlog )
        {
            return mock().listen( backlog );
        }

        auto is_listening() const
        {
            return mock().is_listening();
        }

        auto local_endpoint() const
        {
            return mock().local_endpoint();
        }

        auto accept()
        {
            return mock().accept();
        }

        auto close()
        {
            return mock().close();
        }
    };

    Mock_Acceptor() = default;

    Mock_Acceptor( Mock_Acceptor && ) = delete;

    Mock_Acceptor( Mock_Acceptor const & ) = delete;

    ~Mock_Acceptor() noexcept = default;

    auto operator=( Mock_Acceptor && ) = delete;

    auto operator=( Mock_Acceptor const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), bind, () );
    MOCK_METHOD( (Result<Void, Error_Code>), bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), listen, ( std::uint_fast8_t ) );

    MOCK_METHOD( (Result<bool, Error_Code>), is_listening, (), ( const ) );

    MOCK_METHOD( (Result<::picolibrary::IP::TCP::Endpoint, Error_Code>), local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Server, Error_Code>), accept, () );

    MOCK_METHOD( (Result<Void, Error_Code>), close, () );
};

} // namespace picolibrary::Testing::Unit::IP::TCP

#endif // PICOLIBRARY_TESTING_UNIT_IP_TCP_H
