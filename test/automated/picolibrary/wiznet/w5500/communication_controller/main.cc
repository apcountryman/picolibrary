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
 * \brief picolibrary::WIZnet::W5500::Communication_Controller automated test program.
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::picolibrary::WIZnet::W5500::Memory_Offset;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::Socket_Memory_Block;
using ::testing::A;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;

class Communication_Controller :
    public ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device> {
  public:
    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::Communication_Controller;

    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::read;
    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::write;
};

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Memory_Offset ) works properly.
 */
TEST( readCommonRegisterMemoryByte, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const communication_controller = Communication_Controller{};

    auto const memory_offset = random<Memory_Offset>();
    auto const data          = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller,
        transmit( std::vector<std::uint8_t>{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            0b00000'0'00,
        } ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() );

    EXPECT_EQ( communication_controller.read( memory_offset ), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST( readCommonRegisterMemoryBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const communication_controller = Communication_Controller{};

    auto const memory_offset = random<Memory_Offset>();
    auto const data_expected = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller,
        transmit( std::vector<std::uint8_t>{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            0b00000'0'00,
        } ) );
    EXPECT_CALL( communication_controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( device_selector, deselect() );

    auto data = std::vector<std::uint8_t>( data_expected.size() );
    communication_controller.read( memory_offset, &*data.begin(), &*data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) works properly.
 */
TEST( writeCommonRegisterMemoryByte, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto communication_controller = Communication_Controller{};

    auto const memory_offset = random<Memory_Offset>();
    auto const data          = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller,
        transmit( std::vector<std::uint8_t>{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            0b00000'1'00,
        } ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write( memory_offset, data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t const *, std::uint8_t
 *        const * ) works properly.
 */
TEST( writeCommonRegisterMemoryBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto communication_controller = Communication_Controller{};

    auto const memory_offset = random<Memory_Offset>();
    auto const data          = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller,
        transmit( std::vector<std::uint8_t>{
            static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
            static_cast<std::uint8_t>( memory_offset ),
            0b00000'1'00,
        } ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write( memory_offset, &*data.begin(), &*data.end() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset ) works properly.
 */
TEST( readSocketMemoryByte, worksProperly )
{
    struct {
        Socket_ID           socket_id;
        Socket_Memory_Block socket_memory_block;
        std::uint8_t        control_byte;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, Socket_Memory_Block::REGISTERS, 0b000'01'0'00 },
        { Socket_ID::_0, Socket_Memory_Block::TX_BUFFER, 0b000'10'0'00 },
        { Socket_ID::_0, Socket_Memory_Block::RX_BUFFER, 0b000'11'0'00 },

        { Socket_ID::_1, Socket_Memory_Block::REGISTERS, 0b001'01'0'00 },
        { Socket_ID::_1, Socket_Memory_Block::TX_BUFFER, 0b001'10'0'00 },
        { Socket_ID::_1, Socket_Memory_Block::RX_BUFFER, 0b001'11'0'00 },

        { Socket_ID::_2, Socket_Memory_Block::REGISTERS, 0b010'01'0'00 },
        { Socket_ID::_2, Socket_Memory_Block::TX_BUFFER, 0b010'10'0'00 },
        { Socket_ID::_2, Socket_Memory_Block::RX_BUFFER, 0b010'11'0'00 },

        { Socket_ID::_3, Socket_Memory_Block::REGISTERS, 0b011'01'0'00 },
        { Socket_ID::_3, Socket_Memory_Block::TX_BUFFER, 0b011'10'0'00 },
        { Socket_ID::_3, Socket_Memory_Block::RX_BUFFER, 0b011'11'0'00 },

        { Socket_ID::_4, Socket_Memory_Block::REGISTERS, 0b100'01'0'00 },
        { Socket_ID::_4, Socket_Memory_Block::TX_BUFFER, 0b100'10'0'00 },
        { Socket_ID::_4, Socket_Memory_Block::RX_BUFFER, 0b100'11'0'00 },

        { Socket_ID::_5, Socket_Memory_Block::REGISTERS, 0b101'01'0'00 },
        { Socket_ID::_5, Socket_Memory_Block::TX_BUFFER, 0b101'10'0'00 },
        { Socket_ID::_5, Socket_Memory_Block::RX_BUFFER, 0b101'11'0'00 },

        { Socket_ID::_6, Socket_Memory_Block::REGISTERS, 0b110'01'0'00 },
        { Socket_ID::_6, Socket_Memory_Block::TX_BUFFER, 0b110'10'0'00 },
        { Socket_ID::_6, Socket_Memory_Block::RX_BUFFER, 0b110'11'0'00 },

        { Socket_ID::_7, Socket_Memory_Block::REGISTERS, 0b111'01'0'00 },
        { Socket_ID::_7, Socket_Memory_Block::TX_BUFFER, 0b111'10'0'00 },
        { Socket_ID::_7, Socket_Memory_Block::RX_BUFFER, 0b111'11'0'00 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto device_selector        = Mock_Device_Selector{};
        auto device_selector_handle = device_selector.handle();

        auto const communication_controller = Communication_Controller{};

        auto const memory_offset = random<Memory_Offset>();
        auto const data          = random<std::uint8_t>();

        EXPECT_CALL( communication_controller, configure() );
        EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
        EXPECT_CALL( device_selector, select() );
        EXPECT_CALL(
            communication_controller,
            transmit( std::vector<std::uint8_t>{
                static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
                static_cast<std::uint8_t>( memory_offset ),
                test_case.control_byte,
            } ) );
        EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
        EXPECT_CALL( device_selector, deselect() );

        EXPECT_EQ(
            communication_controller.read( test_case.socket_id, test_case.socket_memory_block, memory_offset ),
            data );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST( readSocketMemoryBlock, worksProperly )
{
    struct {
        Socket_ID           socket_id;
        Socket_Memory_Block socket_memory_block;
        std::uint8_t        control_byte;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, Socket_Memory_Block::REGISTERS, 0b000'01'0'00 },
        { Socket_ID::_0, Socket_Memory_Block::TX_BUFFER, 0b000'10'0'00 },
        { Socket_ID::_0, Socket_Memory_Block::RX_BUFFER, 0b000'11'0'00 },

        { Socket_ID::_1, Socket_Memory_Block::REGISTERS, 0b001'01'0'00 },
        { Socket_ID::_1, Socket_Memory_Block::TX_BUFFER, 0b001'10'0'00 },
        { Socket_ID::_1, Socket_Memory_Block::RX_BUFFER, 0b001'11'0'00 },

        { Socket_ID::_2, Socket_Memory_Block::REGISTERS, 0b010'01'0'00 },
        { Socket_ID::_2, Socket_Memory_Block::TX_BUFFER, 0b010'10'0'00 },
        { Socket_ID::_2, Socket_Memory_Block::RX_BUFFER, 0b010'11'0'00 },

        { Socket_ID::_3, Socket_Memory_Block::REGISTERS, 0b011'01'0'00 },
        { Socket_ID::_3, Socket_Memory_Block::TX_BUFFER, 0b011'10'0'00 },
        { Socket_ID::_3, Socket_Memory_Block::RX_BUFFER, 0b011'11'0'00 },

        { Socket_ID::_4, Socket_Memory_Block::REGISTERS, 0b100'01'0'00 },
        { Socket_ID::_4, Socket_Memory_Block::TX_BUFFER, 0b100'10'0'00 },
        { Socket_ID::_4, Socket_Memory_Block::RX_BUFFER, 0b100'11'0'00 },

        { Socket_ID::_5, Socket_Memory_Block::REGISTERS, 0b101'01'0'00 },
        { Socket_ID::_5, Socket_Memory_Block::TX_BUFFER, 0b101'10'0'00 },
        { Socket_ID::_5, Socket_Memory_Block::RX_BUFFER, 0b101'11'0'00 },

        { Socket_ID::_6, Socket_Memory_Block::REGISTERS, 0b110'01'0'00 },
        { Socket_ID::_6, Socket_Memory_Block::TX_BUFFER, 0b110'10'0'00 },
        { Socket_ID::_6, Socket_Memory_Block::RX_BUFFER, 0b110'11'0'00 },

        { Socket_ID::_7, Socket_Memory_Block::REGISTERS, 0b111'01'0'00 },
        { Socket_ID::_7, Socket_Memory_Block::TX_BUFFER, 0b111'10'0'00 },
        { Socket_ID::_7, Socket_Memory_Block::RX_BUFFER, 0b111'11'0'00 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto device_selector        = Mock_Device_Selector{};
        auto device_selector_handle = device_selector.handle();

        auto const communication_controller = Communication_Controller{};

        auto const memory_offset = random<Memory_Offset>();
        auto const data_expected = random_container<std::vector<std::uint8_t>>();

        EXPECT_CALL( communication_controller, configure() );
        EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
        EXPECT_CALL( device_selector, select() );
        EXPECT_CALL(
            communication_controller,
            transmit( std::vector<std::uint8_t>{
                static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
                static_cast<std::uint8_t>( memory_offset ),
                test_case.control_byte,
            } ) );
        EXPECT_CALL( communication_controller, receive( A<std::vector<std::uint8_t>>() ) )
            .WillOnce( Return( data_expected ) );
        EXPECT_CALL( device_selector, deselect() );

        auto data = std::vector<std::uint8_t>( data_expected.size() );
        communication_controller.read(
            test_case.socket_id, test_case.socket_memory_block, memory_offset, &*data.begin(), &*data.end() );

        EXPECT_EQ( data, data_expected );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) works properly.
 */
TEST( writeSocketMemoryByte, worksProperly )
{
    struct {
        Socket_ID           socket_id;
        Socket_Memory_Block socket_memory_block;
        std::uint8_t        control_byte;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, Socket_Memory_Block::REGISTERS, 0b000'01'1'00 },
        { Socket_ID::_0, Socket_Memory_Block::TX_BUFFER, 0b000'10'1'00 },
        { Socket_ID::_0, Socket_Memory_Block::RX_BUFFER, 0b000'11'1'00 },

        { Socket_ID::_1, Socket_Memory_Block::REGISTERS, 0b001'01'1'00 },
        { Socket_ID::_1, Socket_Memory_Block::TX_BUFFER, 0b001'10'1'00 },
        { Socket_ID::_1, Socket_Memory_Block::RX_BUFFER, 0b001'11'1'00 },

        { Socket_ID::_2, Socket_Memory_Block::REGISTERS, 0b010'01'1'00 },
        { Socket_ID::_2, Socket_Memory_Block::TX_BUFFER, 0b010'10'1'00 },
        { Socket_ID::_2, Socket_Memory_Block::RX_BUFFER, 0b010'11'1'00 },

        { Socket_ID::_3, Socket_Memory_Block::REGISTERS, 0b011'01'1'00 },
        { Socket_ID::_3, Socket_Memory_Block::TX_BUFFER, 0b011'10'1'00 },
        { Socket_ID::_3, Socket_Memory_Block::RX_BUFFER, 0b011'11'1'00 },

        { Socket_ID::_4, Socket_Memory_Block::REGISTERS, 0b100'01'1'00 },
        { Socket_ID::_4, Socket_Memory_Block::TX_BUFFER, 0b100'10'1'00 },
        { Socket_ID::_4, Socket_Memory_Block::RX_BUFFER, 0b100'11'1'00 },

        { Socket_ID::_5, Socket_Memory_Block::REGISTERS, 0b101'01'1'00 },
        { Socket_ID::_5, Socket_Memory_Block::TX_BUFFER, 0b101'10'1'00 },
        { Socket_ID::_5, Socket_Memory_Block::RX_BUFFER, 0b101'11'1'00 },

        { Socket_ID::_6, Socket_Memory_Block::REGISTERS, 0b110'01'1'00 },
        { Socket_ID::_6, Socket_Memory_Block::TX_BUFFER, 0b110'10'1'00 },
        { Socket_ID::_6, Socket_Memory_Block::RX_BUFFER, 0b110'11'1'00 },

        { Socket_ID::_7, Socket_Memory_Block::REGISTERS, 0b111'01'1'00 },
        { Socket_ID::_7, Socket_Memory_Block::TX_BUFFER, 0b111'10'1'00 },
        { Socket_ID::_7, Socket_Memory_Block::RX_BUFFER, 0b111'11'1'00 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto device_selector        = Mock_Device_Selector{};
        auto device_selector_handle = device_selector.handle();

        auto communication_controller = Communication_Controller{};

        auto const memory_offset = random<Memory_Offset>();
        auto const data          = random<std::uint8_t>();

        EXPECT_CALL( communication_controller, configure() );
        EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
        EXPECT_CALL( device_selector, select() );
        EXPECT_CALL(
            communication_controller,
            transmit( std::vector<std::uint8_t>{
                static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
                static_cast<std::uint8_t>( memory_offset ),
                test_case.control_byte,
            } ) );
        EXPECT_CALL( communication_controller, transmit( data ) );
        EXPECT_CALL( device_selector, deselect() );

        communication_controller.write(
            test_case.socket_id, test_case.socket_memory_block, memory_offset, data );
    } // for
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t const *, std::uint8_t
 *        const * ) works properly.
 */
TEST( writeSocketMemoryBlock, worksProperly )
{
    struct {
        Socket_ID           socket_id;
        Socket_Memory_Block socket_memory_block;
        std::uint8_t        control_byte;
    } const test_cases[]{
        // clang-format off

        { Socket_ID::_0, Socket_Memory_Block::REGISTERS, 0b000'01'1'00 },
        { Socket_ID::_0, Socket_Memory_Block::TX_BUFFER, 0b000'10'1'00 },
        { Socket_ID::_0, Socket_Memory_Block::RX_BUFFER, 0b000'11'1'00 },

        { Socket_ID::_1, Socket_Memory_Block::REGISTERS, 0b001'01'1'00 },
        { Socket_ID::_1, Socket_Memory_Block::TX_BUFFER, 0b001'10'1'00 },
        { Socket_ID::_1, Socket_Memory_Block::RX_BUFFER, 0b001'11'1'00 },

        { Socket_ID::_2, Socket_Memory_Block::REGISTERS, 0b010'01'1'00 },
        { Socket_ID::_2, Socket_Memory_Block::TX_BUFFER, 0b010'10'1'00 },
        { Socket_ID::_2, Socket_Memory_Block::RX_BUFFER, 0b010'11'1'00 },

        { Socket_ID::_3, Socket_Memory_Block::REGISTERS, 0b011'01'1'00 },
        { Socket_ID::_3, Socket_Memory_Block::TX_BUFFER, 0b011'10'1'00 },
        { Socket_ID::_3, Socket_Memory_Block::RX_BUFFER, 0b011'11'1'00 },

        { Socket_ID::_4, Socket_Memory_Block::REGISTERS, 0b100'01'1'00 },
        { Socket_ID::_4, Socket_Memory_Block::TX_BUFFER, 0b100'10'1'00 },
        { Socket_ID::_4, Socket_Memory_Block::RX_BUFFER, 0b100'11'1'00 },

        { Socket_ID::_5, Socket_Memory_Block::REGISTERS, 0b101'01'1'00 },
        { Socket_ID::_5, Socket_Memory_Block::TX_BUFFER, 0b101'10'1'00 },
        { Socket_ID::_5, Socket_Memory_Block::RX_BUFFER, 0b101'11'1'00 },

        { Socket_ID::_6, Socket_Memory_Block::REGISTERS, 0b110'01'1'00 },
        { Socket_ID::_6, Socket_Memory_Block::TX_BUFFER, 0b110'10'1'00 },
        { Socket_ID::_6, Socket_Memory_Block::RX_BUFFER, 0b110'11'1'00 },

        { Socket_ID::_7, Socket_Memory_Block::REGISTERS, 0b111'01'1'00 },
        { Socket_ID::_7, Socket_Memory_Block::TX_BUFFER, 0b111'10'1'00 },
        { Socket_ID::_7, Socket_Memory_Block::RX_BUFFER, 0b111'11'1'00 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto device_selector        = Mock_Device_Selector{};
        auto device_selector_handle = device_selector.handle();

        auto communication_controller = Communication_Controller{};

        auto const memory_offset = random<Memory_Offset>();
        auto const data          = random_container<std::vector<std::uint8_t>>();

        EXPECT_CALL( communication_controller, configure() );
        EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
        EXPECT_CALL( device_selector, select() );
        EXPECT_CALL(
            communication_controller,
            transmit( std::vector<std::uint8_t>{
                static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
                static_cast<std::uint8_t>( memory_offset ),
                test_case.control_byte,
            } ) );
        EXPECT_CALL( communication_controller, transmit( data ) );
        EXPECT_CALL( device_selector, deselect() );

        communication_controller.write(
            test_case.socket_id, test_case.socket_memory_block, memory_offset, &*data.begin(), &*data.end() );
    } // for
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Communication_Controller automated
 *        tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argc The array of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
