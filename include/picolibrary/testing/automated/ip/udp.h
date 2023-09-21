/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Testing::Automated::IP::UDP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_IP_UDP_H
#define PICOLIBRARY_TESTING_AUTOMATED_IP_UDP_H

#include <cstdint>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/ip/udp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/mock_handle.h"

/**
 * \brief UDP over IP automated testing facilities.
 */
namespace picolibrary::Testing::Automated::IP::UDP {

/**
 * \brief Mock socket.
 */
class Mock_Socket {
  public:
    class Handle : public Mock_Handle<Mock_Socket> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Socket & mock ) noexcept : Mock_Handle<Mock_Socket>{ mock }
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

        void bind( ::picolibrary::IP::UDP::Endpoint const & endpoint )
        {
            mock().bind( endpoint );
        }

        auto local_endpoint() const -> ::picolibrary::IP::UDP::Endpoint
        {
            return mock().local_endpoint();
        }

        auto transmit(
            ::picolibrary::IP::UDP::Endpoint const & endpoint,
            std::uint8_t const *                     begin,
            std::uint8_t const *                     end ) -> Result<void>
        {
            return mock().transmit( endpoint, begin, end );
        }

        auto receive( std::uint8_t * begin, std::uint8_t * end )
            -> Result<::picolibrary::IP::UDP::Reception_Result>
        {
            return mock().receive( begin, end );
        }

        void close()
        {
            mock().close();
        }
    };

    Mock_Socket() = default;

    Mock_Socket( Mock_Socket && ) = delete;

    Mock_Socket( Mock_Socket const & ) = delete;

    ~Mock_Socket() noexcept = default;

    auto operator=( Mock_Socket && ) = delete;

    auto operator=( Mock_Socket const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::UDP::Endpoint const &));

    MOCK_METHOD( ::picolibrary::IP::UDP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Result<void>, transmit, (::picolibrary::IP::UDP::Endpoint const &, std::vector<std::uint8_t>));

    auto transmit( ::picolibrary::IP::UDP::Endpoint const & endpoint, std::uint8_t const * begin, std::uint8_t const * end )
        -> Result<void>
    {
        return transmit( endpoint, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<std::pair<::picolibrary::IP::UDP::Endpoint, std::vector<std::uint8_t>>>), receive, () );

    auto receive( std::uint8_t * begin, std::uint8_t * end )
        -> Result<::picolibrary::IP::UDP::Reception_Result>
    {
        static_cast<void>( end );

        auto const result = receive();
        if ( result.is_error() ) {
            return result.error();
        } // if

        for ( auto const data : result.value().second ) {
            *begin = data;

            ++begin;
        } // for

        return ::picolibrary::IP::UDP::Reception_Result{ result.value().first, begin };
    }

    MOCK_METHOD( void, close, () );
};

} // namespace picolibrary::Testing::Automated::IP::UDP

#endif // PICOLIBRARY_TESTING_AUTOMATED_IP_UDP_H
