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
 * \brief picolibrary::Microchip::MCP3008::Driver automated test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <ostream>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/automated/adc.h"
#include "picolibrary/testing/automated/microchip/mcp3008.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Microchip::MCP3008::Sample;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

using Driver = ::picolibrary::Microchip::MCP3008::Driver<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>;

} // namespace

/**
 * \brief picolibrary::Microchip::MCP3008::Driver::sample() test case.
 */
struct sample_Test_Case {
    /**
     * \brief The input to get the sample from.
     */
    Input input;

    /**
     * \brief The data transmitted to the MCP3008.
     */
    std::vector<std::uint8_t> tx;

    /**
     * \brief The data received from the MCP3008.
     */
    std::vector<std::uint8_t> rx;

    /**
     * \brief The sample.
     */
    Sample sample;
};

auto operator<<( std::ostream & stream, sample_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".input = " << test_case.input
                  << ", "
                  << ".tx = { " << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.tx[ 0 ] ) << ", "
                                << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.tx[ 1 ] ) << ", "
                                << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.tx[ 2 ] ) << " }"
                  << ", "
                  << ".rx = { " << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.rx[ 0 ] ) << ", "
                                << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.rx[ 1 ] ) << ", "
                                << "0x" << std::hex << std::uppercase << std::setw( std::numeric_limits<std::uint8_t>::digits / 4 ) << std::setfill( '0' ) << static_cast<std::uint_fast16_t>( test_case.rx[ 2 ] ) << " }"
                  << ", "
                  << ".sample = " << test_case.sample
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::Microchip::MCP3008::Driver::sample() test fixture.
 */
class sample : public TestWithParam<sample_Test_Case> {
};

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() works properly.
 */
TEST_P( sample, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto controller             = Mock_Controller{};
    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto mcp3008 = Driver{ controller, 39, device_selector.handle() };

    EXPECT_CALL( mcp3008, configure() );
    EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( mcp3008, exchange( test_case.tx ) ).WillOnce( Return( test_case.rx ) );
    EXPECT_CALL( device_selector, deselect() );

    EXPECT_EQ( mcp3008.sample( test_case.input ), test_case.sample );
}

/**
 * \brief picolibrary::Microchip::MCP3008::Driver::sample() test cases.
 */
sample_Test_Case const sample_TEST_CASES[]{
    // clang-format off

    { Input::CH0, { 0x01, 0b1'000'0000, 0x00 }, { 0x5D, 0b000011'00 | 0x2, 0xEC }, 0x2EC },
    { Input::CH1, { 0x01, 0b1'001'0000, 0x00 }, { 0xD2, 0b110100'00 | 0x1, 0x05 }, 0x105 },
    { Input::CH2, { 0x01, 0b1'010'0000, 0x00 }, { 0x8A, 0b111110'00 | 0x2, 0x5C }, 0x25C },
    { Input::CH3, { 0x01, 0b1'011'0000, 0x00 }, { 0x05, 0b010101'00 | 0x3, 0x74 }, 0x374 },
    { Input::CH4, { 0x01, 0b1'100'0000, 0x00 }, { 0xAA, 0b000100'00 | 0x3, 0xB9 }, 0x3B9 },
    { Input::CH5, { 0x01, 0b1'101'0000, 0x00 }, { 0x16, 0b111000'00 | 0x3, 0x03 }, 0x303 },
    { Input::CH6, { 0x01, 0b1'110'0000, 0x00 }, { 0x03, 0b001101'00 | 0x0, 0xF9 }, 0x0F9 },
    { Input::CH7, { 0x01, 0b1'111'0000, 0x00 }, { 0x8A, 0b010000'00 | 0x1, 0x40 }, 0x140 },

    { Input::CH0_RELATIVE_TO_CH1, { 0x01, 0b0'000'0000, 0x00 }, { 0x79, 0b000111'00 | 0x3, 0x07 }, 0x307 },
    { Input::CH1_RELATIVE_TO_CH0, { 0x01, 0b0'001'0000, 0x00 }, { 0x08, 0b111011'00 | 0x1, 0x39 }, 0x139 },
    { Input::CH2_RELATIVE_TO_CH3, { 0x01, 0b0'010'0000, 0x00 }, { 0xEA, 0b010100'00 | 0x1, 0x7D }, 0x17D },
    { Input::CH3_RELATIVE_TO_CH2, { 0x01, 0b0'011'0000, 0x00 }, { 0xA0, 0b000100'00 | 0x2, 0x15 }, 0x215 },
    { Input::CH4_RELATIVE_TO_CH5, { 0x01, 0b0'100'0000, 0x00 }, { 0x9E, 0b101011'00 | 0x2, 0xED }, 0x2ED },
    { Input::CH5_RELATIVE_TO_CH4, { 0x01, 0b0'101'0000, 0x00 }, { 0x03, 0b010100'00 | 0x1, 0xD9 }, 0x1D9 },
    { Input::CH6_RELATIVE_TO_CH7, { 0x01, 0b0'110'0000, 0x00 }, { 0x74, 0b110111'00 | 0x2, 0x08 }, 0x208 },
    { Input::CH7_RELATIVE_TO_CH6, { 0x01, 0b0'111'0000, 0x00 }, { 0x97, 0b101011'00 | 0x3, 0xB4 }, 0x3B4 },

    { Input::CH2, { 0x01, 0b1'010'0000, 0x00 }, { 0x4A, 0b001001'00 | 0x0, 0x00 }, 0x000 },
    { Input::CH7, { 0x01, 0b1'111'0000, 0x00 }, { 0x89, 0b100101'00 | 0x3, 0xFF }, 0x3FF },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, sample, ValuesIn( sample_TEST_CASES ) );

/**
 * \brief Execute the picolibrary::Microchip::MCP3008::Driver automated tests.
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
