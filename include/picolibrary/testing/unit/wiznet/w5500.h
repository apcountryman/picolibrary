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
 * \brief picolibrary::Testing::Unit::WIZnet::W5500 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H
#define PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H

#include <algorithm>
#include <cstdint>
#include <vector>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random WIZnet W5500 socket ID.
 *
 * \return A pseudo-randomly generated WIZnet W5500 socket ID.
 */
template<>
inline auto random<WIZnet::W5500::Socket_ID>()
{
    return static_cast<WIZnet::W5500::Socket_ID>( random<std::uint8_t>(
        static_cast<std::uint8_t>( WIZnet::W5500::Socket_ID::_0 ),
        static_cast<std::uint8_t>( WIZnet::W5500::Socket_ID::_7 ) ) );
}

/**
 * \brief Generate a pseudo-random WIZnet W5500 region.
 *
 * \return A pseudo-randomly generated WIZnet W5500 region.
 */
template<>
inline auto random<WIZnet::W5500::Region>()
{
    return static_cast<WIZnet::W5500::Region>( random<std::uint8_t>(
        static_cast<std::uint8_t>( WIZnet::W5500::Region::REGISTERS ),
        static_cast<std::uint8_t>( WIZnet::W5500::Region::RX_BUFFER ) ) );
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief WIZnet W5500 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::WIZnet::W5500 {

/**
 * \brief Socket ID.
 */
using Socket_ID = ::picolibrary::WIZnet::W5500::Socket_ID;

/**
 * \brief Region.
 */
using Region = ::picolibrary::WIZnet::W5500::Region;

/**
 * \brief WIZnet W5500 mock communication controller.
 */
class Mock_Communication_Controller : public SPI::Mock_Device {
  public:
    /**
     * \brief The type of SPI controller used to communicate with the W5500.
     */
    using Controller = SPI::Mock_Controller;

    /**
     * \brief The type of SPI device selector used to select and deselect the W5500.
     */
    using Device_Selector = SPI::Mock_Device_Selector::Handle;

    /**
     * \brief Constructor.
     */
    Mock_Communication_Controller() = default;

    /**
     * \brief Constructor.
     */
    Mock_Communication_Controller( Controller &, Controller::Configuration, Device_Selector )
    {
    }

    Mock_Communication_Controller( Mock_Communication_Controller && ) = delete;

    Mock_Communication_Controller( Mock_Communication_Controller const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Communication_Controller() noexcept = default;

    auto operator=( Mock_Communication_Controller && ) = delete;

    auto operator=( Mock_Communication_Controller const & ) = delete;

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( std::uint16_t ), ( const ) );

    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (std::uint16_t, std::vector<std::uint8_t>),
        ( const ) );

    /**
     * \brief Read a block of common register memory.
     *
     * \param[in] offset The offset of the block of register memory to read.
     * \param[in] begin The beginning of the data read from the block of register memory.
     * \param[in] end The end of the data read from the block of register memory.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( offset, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( std::uint16_t, std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, (std::uint16_t, std::vector<std::uint8_t>));

    /**
     * \brief Write to a block of common register memory.
     *
     * \param[in] offset The offset of the block of register memory to write to.
     * \param[in] begin The beginning of the data to write to the block of register
     *            memory.
     * \param[in] end The end of the data to write to the block of register memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( offset, std::vector<std::uint8_t>{ begin, end } );
    }

    MOCK_METHOD( (Result<std::uint8_t, Error_Code>), read, ( Socket_ID, Region, std::uint16_t ), ( const ) );

    MOCK_METHOD(
        (Result<std::vector<std::uint8_t>, Error_Code>),
        read,
        (Socket_ID, Region, std::uint16_t, std::vector<std::uint8_t>),
        ( const ) );

    /**
     * \brief Read a block of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            read.
     * \param[in] region The memory region to read.
     * \param[in] offset The offset of the block of register or buffer memory to read.
     * \param[in] begin The beginning of the data read from the block of register or
     *            buffer memory.
     * \param[in] end The end of the data read from the block of register or buffer
     *            memory.
     *
     * \return Nothing if the read succeeded.
     * \return An error code if the read failed.
     */
    auto read( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t * begin, std::uint8_t * end ) const
        -> Result<Void, Error_Code>
    {
        static_cast<void>( end );

        auto const result = read( socket_id, region, offset, std::vector<std::uint8_t>{} );

        if ( result.is_error() ) {
            return result.error();
        } // if

        std::for_each( result.value().begin(), result.value().end(), [ &begin ]( auto data ) {
            *begin = data;

            ++begin;
        } );

        return {};
    }

    MOCK_METHOD( (Result<Void, Error_Code>), write, ( Socket_ID, Region, std::uint16_t, std::uint8_t ) );

    MOCK_METHOD( (Result<Void, Error_Code>), write, (Socket_ID, Region, std::uint16_t, std::vector<std::uint8_t>));

    /**
     * \brief Write to a block of socket register or buffer memory.
     *
     * \param[in] socket_id The ID of the socket whose register or buffer memory will be
     *            written to.
     * \param[in] region The memory region to write to.
     * \param[in] offset The offset of the block of register or buffer memory to write to.
     * \param[in] begin The beginning of the data to write to the block of register or
     *            buffer memory.
     * \param[in] end The end of the data to write to the block of register or buffer
     *            memory.
     *
     * \return Nothing if the write succeeded.
     * \return An error code if the write failed.
     */
    auto write( Socket_ID socket_id, Region region, std::uint16_t offset, std::uint8_t const * begin, std::uint8_t const * end )
    {
        return write( socket_id, region, offset, std::vector<std::uint8_t>{ begin, end } );
    }
};

} // namespace picolibrary::Testing::Unit::WIZnet::W5500

#endif // PICOLIBRARY_TESTING_UNIT_WIZNET_W5500_H
