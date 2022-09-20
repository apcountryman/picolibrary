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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H

#include <cstdint>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 Transmission Control Protocol (TCP) over IP automated testing
 *        facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP {

/**
 * \brief Mock client socket.
 */
class Mock_Client {
  public:
    using Size = std::uint16_t;

    enum class State : std::uint_fast8_t {
        UNINITIALIZED,
        INITIALIZED,
        BOUND,
        CONNECTING,
        CONNECTED,
    };

    class Handle : public Mock_Handle<Mock_Client> {
      public:
        using Size = Mock_Client::Size;

        using State = Mock_Client::State;

        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Client & mock ) noexcept : Mock_Handle<Mock_Client>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto state() const -> State
        {
            return mock().state();
        }

        auto socket_id() const -> ::picolibrary::WIZnet::W5500::Socket_ID
        {
            return mock().socket_id();
        }

        auto socket_interrupt_mask() const -> std::uint8_t
        {
            return mock().socket_interrupt_mask();
        }

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

    MOCK_METHOD( State, state, (), ( const ) );

    MOCK_METHOD( ::picolibrary::WIZnet::W5500::Socket_ID, socket_id, (), ( const ) );

    MOCK_METHOD( std::uint8_t, socket_interrupt_mask, (), ( const ) );

    MOCK_METHOD( void, bind, () );
    MOCK_METHOD( void, bind, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( (Result<Void, Error_Code>), connect, (::picolibrary::IP::TCP::Endpoint const &));

    MOCK_METHOD( bool, is_connected, (), ( const ) );

    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, remote_endpoint, (), ( const ) );
    MOCK_METHOD( ::picolibrary::IP::TCP::Endpoint, local_endpoint, (), ( const ) );

    MOCK_METHOD( Size, outstanding, (), ( const ) );

    MOCK_METHOD( void, close, () );
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500::IP::TCP

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_TCP_H
