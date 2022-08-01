/**
 * picolibrary
 *
 * Copyright 2020-2022, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Automated::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_TCP_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_TCP_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/void.h"

/**
 * \brief Transmission Control Protocol (TCP) over IP automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IP::TCP {

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

        auto operator=( Handle const & ) = delete;

        void bind()
        {
            mock().bind();
        }

        void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        auto connect( ::picolibrary::IP::TCP::Endpoint const & endpoint ) -> Result<Void, Error_Code>
        {
            return mock().connect( endpoint );
        }

        auto is_connected() const -> bool
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto outstanding() const -> Size
        {
            return mock().outstanding();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
            -> Result<std::uint8_t const *, Error_Code>
        {
            return mock().transmit( begin, end );
        }

        auto available() const -> Size
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
        {
            return mock().receive( begin, end );
        }

        void shutdown()
        {
            mock().shutdown();
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Client() = default;

    Mock_Client( Mock_Client && ) = delete;

    Mock_Client( Mock_Client const & ) = delete;

    ~Mock_Client() noexcept = default;

    auto operator=( Mock_Client && ) = delete;

    auto operator=( Mock_Client const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( bool, is_connected, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, remote_endpoint, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Size, outstanding, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( Size, available, (), ( const ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value() ) {
            *begin = data;

            ++begin;
        } // for

        return begin;
    }

    MOCK_METHOD( void, shutdown, () );

    MOCK_METHOD( void, close, () );
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

        auto operator=( Handle const & ) = delete;

        auto is_connected() const -> bool
        {
            return mock().is_connected();
        }

        auto remote_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().remote_endpoint();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto outstanding() const -> Size
        {
            return mock().outstanding();
        }

        auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
            -> Result<std::uint8_t const *, Error_Code>
        {
            return mock().transmit( begin, end );
        }

        auto available() const -> Size
        {
            return mock().available();
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
        {
            return mock().receive( begin, end );
        }

        void shutdown()
        {
            mock().shutdown();
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Server() = default;

    Mock_Server( Mock_Server && ) = delete;

    Mock_Server( Mock_Server const & ) = delete;

    ~Mock_Server() noexcept = default;

    auto operator=( Mock_Server && ) = delete;

    auto operator=( Mock_Server const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( bool, is_connected, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, remote_endpoint, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Size, outstanding, (), ( const ) );

    MOCK_METHOD( (Result<std::uint8_t const *, Error_Code>), transmit, (std::vector<std::uint8_t>));

    auto transmit( std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<std::uint8_t const *, Error_Code>
    {
        return transmit( std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( Size, available, (), ( const ) );

    MOCK_METHOD( (Result<std::vector<std::uint8_t>, Error_Code>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end ) -> Result<std::uint8_t *, Error_Code>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value() ) {
            *begin = data;

            ++begin;
        } // for

        return begin;
    }

    MOCK_METHOD( void, shutdown, () );

    MOCK_METHOD( void, close, () );
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

        auto operator=( Handle const & ) = delete;

        void bind()
        {
            mock().bind();
        }

        void bind( ::picolibrary::IP::TCP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        void listen( std::uint_fast8_t backlog )
        {
            mock().listen( backlog );
        }

        auto is_listening() const -> bool
        {
            return mock().is_listening();
        }

        auto local_endpoint() const -> ::picolibrary::IP::TCP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto accept() -> Result<Server, Error_Code>
        {
            return mock().accept();
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Acceptor() = default;

    Mock_Acceptor( Mock_Acceptor && ) = delete;

    Mock_Acceptor( Mock_Acceptor const & ) = delete;

    ~Mock_Acceptor() noexcept = default;

    auto operator=( Mock_Acceptor && ) = delete;

    auto operator=( Mock_Acceptor const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( void, listen, ( std::uint_fast8_t ) );

    MOCK_METHOD( bool, is_listening, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( (Result<Server, Error_Code>), accept, () );

    MOCK_METHOD( void, close, () );
};

} // namespace picolibrary::Testing::Automated::IP::TCP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_TCP_H
