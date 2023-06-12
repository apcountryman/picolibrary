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
 * \brief picolibrary::WIZnet::W5500::Communication_Controller automated test program.
 */

#include <bitset>
#include <cstdint>
#include <limits>
#include <ostream>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/testing/automated/spi.h"
#include "picolibrary/testing/automated/wiznet/w5500.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

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
using ::testing::TestWithParam;
using ::testing::ValuesIn;

class Communication_Controller :
    public ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device> {
  public:
    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::Communication_Controller;

    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::read;

    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::write;
};

auto frame_header( Memory_Offset memory_offset, std::uint8_t control_byte ) -> std::vector<std::uint8_t>
{
    return { static_cast<std::uint8_t>( memory_offset >> std::numeric_limits<std::uint8_t>::digits ),
             static_cast<std::uint8_t>( memory_offset ),
             control_byte };
}

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

    auto const memory_offset = Memory_Offset{ 0x7790 };
    auto const data          = std::uint8_t{ 0x8B };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( frame_header( memory_offset, 0b000'00'0'00 ) ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() );

    ASSERT_EQ( communication_controller.read( memory_offset ), data );
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

    auto const memory_offset = Memory_Offset{ 0x7790 };
    auto       data          = std::vector<std::uint8_t>( 4 );
    auto const data_expected = std::vector<std::uint8_t>{ 0x61, 0x9A, 0x1D, 0x39 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( frame_header( memory_offset, 0b000'00'0'00 ) ) );
    EXPECT_CALL( communication_controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.read( memory_offset, &*data.begin(), &*data.end() );

    ASSERT_EQ( data, data_expected );
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

    auto const memory_offset = Memory_Offset{ 0xD0C2 };
    auto const data          = std::uint8_t{ 0x70 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( frame_header( memory_offset, 0b000'00'1'00 ) ) );
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

    auto const memory_offset = Memory_Offset{ 0xAD97 };
    auto const data          = std::vector<std::uint8_t>{ 0xFC, 0xCC, 0x5F, 0x71, 0x14 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( frame_header( memory_offset, 0b000'00'1'00 ) ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write( memory_offset, &*data.begin(), &*data.end() );
}

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset ), and
 *        picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * ),
 *        picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block, std::uint8_t ), and
 *        picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block, std::uint8_t const *,
 *        std::uint8_t const * ) test case.
 *
 */
struct socketMemory_Test_Case {
    /**
     * \brief The ID of the socket.
     */
    Socket_ID socket_id;

    /**
     * \brief The socket memory block.
     */
    Socket_Memory_Block socket_memory_block;

    /**
     * \brief The control byte.
     */
    std::uint8_t control_byte;
};

auto operator<<( std::ostream & stream, socketMemory_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".socket_id = " << test_case.socket_id
                  << ", "
                  << ".socket_memory_block = " << test_case.socket_memory_block
                  << ", "
                  << ".control_byte = 0b" << std::bitset<std::numeric_limits<std::uint8_t>::digits>{ test_case.control_byte }
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset ), and
 *        picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * ) test
 *        cases.
 */
socketMemory_Test_Case const readSocketMemory_TEST_CASES[]{
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

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset ) test fixture.
 */
class readSocketMemoryByte : public TestWithParam<socketMemory_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset ) works properly.
 */
TEST_P( readSocketMemoryByte, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const communication_controller = Communication_Controller{};

    auto const memory_offset = Memory_Offset{ 0xE6AA };
    auto const data          = std::uint8_t{ 0x3E };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller, transmit( frame_header( memory_offset, test_case.control_byte ) ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() );

    ASSERT_EQ(
        communication_controller.read( test_case.socket_id, test_case.socket_memory_block, memory_offset ),
        data );
}

INSTANTIATE_TEST_SUITE_P( testCases, readSocketMemoryByte, ValuesIn( readSocketMemory_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * ) test
 *        fixture.
 */
class readSocketMemoryBlock : public TestWithParam<socketMemory_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST_P( readSocketMemoryBlock, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const communication_controller = Communication_Controller{};

    auto const memory_offset = Memory_Offset{ 0x809F };
    auto       data          = std::vector<std::uint8_t>( 5 );
    auto const data_expected = std::vector<std::uint8_t>{ 0xDF, 0x54, 0xBB, 0xA9, 0x0C };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller, transmit( frame_header( memory_offset, test_case.control_byte ) ) );
    EXPECT_CALL( communication_controller, receive( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.read(
        test_case.socket_id, test_case.socket_memory_block, memory_offset, &*data.begin(), &*data.end() );

    ASSERT_EQ( data, data_expected );
}

INSTANTIATE_TEST_SUITE_P( testCases, readSocketMemoryBlock, ValuesIn( readSocketMemory_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ), and
 *        picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t const *, std::uint8_t
 *        const * ) test cases.
 */
socketMemory_Test_Case const writeSocketMemory_TEST_CASES[]{
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

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) test fixture.
 */
class writeSocketMemoryByte : public TestWithParam<socketMemory_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t ) works properly.
 */
TEST_P( writeSocketMemoryByte, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto communication_controller = Communication_Controller{};

    auto const memory_offset = Memory_Offset{ 0xD7F1 };
    auto const data          = std::uint8_t{ 0xA8 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller, transmit( frame_header( memory_offset, test_case.control_byte ) ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write(
        test_case.socket_id, test_case.socket_memory_block, memory_offset, data );
}

INSTANTIATE_TEST_SUITE_P( testCases, writeSocketMemoryByte, ValuesIn( writeSocketMemory_TEST_CASES ) );

/**
 * \brief picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t const *, std::uint8_t
 *        const * ) test fixture.
 */
class writeSocketMemoryBlock : public TestWithParam<socketMemory_Test_Case> {
};

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        picolibrary::WIZnet::W5500::Socket_ID,
 *        picolibrary::WIZnet::W5500::Socket_Memory_Block,
 *        picolibrary::WIZnet::W5500::Memory_Offset, std::uint8_t const *, std::uint8_t
 *        const * ) works properly.
 */
TEST_P( writeSocketMemoryBlock, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto communication_controller = Communication_Controller{};

    auto const memory_offset = Memory_Offset{ 0xB6EB };
    auto const data = std::vector<std::uint8_t>{ 0xBB, 0x7F, 0x5E, 0x36, 0x3E, 0x7E };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL(
        communication_controller, transmit( frame_header( memory_offset, test_case.control_byte ) ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write(
        test_case.socket_id, test_case.socket_memory_block, memory_offset, &*data.begin(), &*data.end() );
}

INSTANTIATE_TEST_SUITE_P( testCases, writeSocketMemoryBlock, ValuesIn( writeSocketMemory_TEST_CASES ) );

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
