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
 * \brief picolibrary::Testing::Automated::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Automated {

/**
 * \brief Generate a pseudo-random picolibrary::WIZnet::W5500::Socket_ID.
 *
 * \return A pseudo-randomly generated picolibrary::WIZnet::W5500::Socket_ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>() -> WIZnet::W5500::Socket_ID
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint_fast8_t>( 0, 7 ) << 5 );
}

/**
 * \brief Generate a pseudo-random picolibrary::WIZnet::W5500::Socket_Memory_Block.
 *
 * \return A pseudo-randomly generated picolibrary::WIZnet::W5500::Socket_Memory_Block.
 */
template<>
inline auto random<WIZnet::W5500::Socket_Memory_Block>() -> WIZnet::W5500::Socket_Memory_Block
{
    return static_cast<WIZnet::W5500::Socket_Memory_Block>(
        random<std::uint_fast8_t>( 0b01, 0b11 ) << 3 );
}

} // namespace picolibrary::Testing::Automated

/**
 * \brief WIZnet W5500 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::WIZnet::W5500 {

/**
 * \brief Mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    Mock_Communication_Controller() noexcept
    {
    }

    Mock_Communication_Controller(
        SPI::Mock_Controller &,
        SPI::Mock_Controller::Configuration const &,
        SPI::Mock_Device_Selector::Handle ) noexcept
    {
    }

    Mock_Communication_Controller( Mock_Communication_Controller && ) = delete;

    Mock_Communication_Controller( Mock_Communication_Controller const & ) = delete;

    ~Mock_Communication_Controller() noexcept = default;

    auto operator=( Mock_Communication_Controller && ) = delete;

    auto operator=( Mock_Communication_Controller const & ) = delete;

    MOCK_METHOD( std::uint8_t, read, ( ::picolibrary::WIZnet::W5500::Memory_Offset ), ( const ) );
    MOCK_METHOD(
        std::vector<std::uint8_t>,
        read,
        (::picolibrary::WIZnet::W5500::Memory_Offset, std::vector<std::uint8_t>),
        ( const ) );

    void read( ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset, std::uint8_t * begin, std::uint8_t * end ) const
    {
        static_cast<void>( end );

        auto const data = read( memory_offset, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD( void, write, ( ::picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) );
    MOCK_METHOD( void, write, (::picolibrary::WIZnet::W5500::Memory_Offset, std::vector<std::uint8_t>));

    void write( ::picolibrary::WIZnet::W5500::Memory_Offset memory_offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        write( memory_offset, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD(
        std::uint8_t,
        read,
        ( ::picolibrary::WIZnet::W5500::Socket_ID,
          ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
          ::picolibrary::WIZnet::W5500::Memory_Offset ),
        ( const ) );
    MOCK_METHOD(
        std::vector<std::uint8_t>,
        read,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>),
        ( const ) );

    void read(
        ::picolibrary::WIZnet::W5500::Socket_ID           socket_id,
        ::picolibrary::WIZnet::W5500::Socket_Memory_Block socket_memory_block,
        ::picolibrary::WIZnet::W5500::Memory_Offset       memory_offset,
        std::uint8_t *                                    begin,
        std::uint8_t *                                    end ) const
    {
        static_cast<void>( end );

        auto const data = read(
            socket_id, socket_memory_block, memory_offset, std::vector<std::uint8_t>{} );

        std::copy( data.begin(), data.end(), begin );
    }

    MOCK_METHOD(
        void,
        write,
        ( ::picolibrary::WIZnet::W5500::Socket_ID,
          ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
          ::picolibrary::WIZnet::W5500::Memory_Offset,
          std::uint8_t ) );
    MOCK_METHOD(
        void,
        write,
        (::picolibrary::WIZnet::W5500::Socket_ID,
         ::picolibrary::WIZnet::W5500::Socket_Memory_Block,
         ::picolibrary::WIZnet::W5500::Memory_Offset,
         std::vector<std::uint8_t>));

    void write(
        ::picolibrary::WIZnet::W5500::Socket_ID           socket_id,
        ::picolibrary::WIZnet::W5500::Socket_Memory_Block socket_memory_block,
        ::picolibrary::WIZnet::W5500::Memory_Offset       memory_offset,
        std::uint8_t const *                              begin,
        std::uint8_t const *                              end )
    {
        write( socket_id, socket_memory_block, memory_offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Automated::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_AUTOMATED_WIZNET_W5500_H
