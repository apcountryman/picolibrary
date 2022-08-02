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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500::IP interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_H

#include "gmock/gmock.h"
#include "picolibrary/ip.h"
#include "picolibrary/testing/automated/mock_handle.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"

/**
 * \brief WIZnet W5500 Internet Protocol (IP) automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500::IP {

/**
 * \brief Mock protocol port allocator.
 */
class Mock_Port_Allocator {
  public:
    class Handle : public Mock_Handle<Mock_Port_Allocator> {
      public:
        constexpr Handle() noexcept = default;

        constexpr Handle( Mock_Port_Allocator & mock ) noexcept :
            Mock_Handle<Mock_Port_Allocator>{ mock }
        {
        }

        constexpr Handle( Handle && source ) noexcept = default;

        Handle( Handle const & ) = delete;

        ~Handle() noexcept = default;

        constexpr auto operator=( Handle && expression ) noexcept -> Handle & = default;

        auto operator=( Handle const & ) = delete;

        auto allocate( Mock_Driver const & driver, ::picolibrary::IP::Port port )
            -> ::picolibrary::IP::Port
        {
            return mock().allocate( driver, port );
        }

        void deallocate( ::picolibrary::IP::Port port )
        {
            mock().deallocate( port );
        }
    };

    Mock_Port_Allocator() = default;

    Mock_Port_Allocator( Mock_Port_Allocator && ) = delete;

    Mock_Port_Allocator( Mock_Port_Allocator const & ) = delete;

    ~Mock_Port_Allocator() noexcept = default;

    auto operator=( Mock_Port_Allocator && ) = delete;

    auto operator=( Mock_Port_Allocator const & ) = delete;

    auto handle() noexcept -> Handle
    {
        return Handle{ *this };
    }

    MOCK_METHOD( ::picolibrary::IP::Port, allocate, ( Mock_Driver const &, ::picolibrary::IP::Port ) );

    MOCK_METHOD( void, deallocate, ( ::picolibrary::IP::Port ) );
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500::IP

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_IP_H
