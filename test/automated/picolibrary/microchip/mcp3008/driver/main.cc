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
 * \brief picolibrary::Microchip::MCP3008::Driver automated test program.
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/testing/automated/adc.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::Microchip::MCP3008::Input;
using ::picolibrary::Microchip::MCP3008::Sample;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;

using Driver = ::picolibrary::Microchip::MCP3008::Driver<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>;

} // namespace

/**
 * \brief Verify picolibrary::Microchip::MCP3008::Driver::sample() works properly.
 */
TEST( sample, worksProperly )
{
    struct {
        Input        input;
        std::uint8_t control_byte;
    } const test_cases[]{
        // clang-format off

        { Input::CH0, 0b1'000'0000 },
        { Input::CH1, 0b1'001'0000 },
        { Input::CH2, 0b1'010'0000 },
        { Input::CH3, 0b1'011'0000 },
        { Input::CH4, 0b1'100'0000 },
        { Input::CH5, 0b1'101'0000 },
        { Input::CH6, 0b1'110'0000 },
        { Input::CH7, 0b1'111'0000 },

        { Input::CH0_RELATIVE_TO_CH1, 0b0'000'0000 },
        { Input::CH1_RELATIVE_TO_CH0, 0b0'001'0000 },
        { Input::CH2_RELATIVE_TO_CH3, 0b0'010'0000 },
        { Input::CH3_RELATIVE_TO_CH2, 0b0'011'0000 },
        { Input::CH4_RELATIVE_TO_CH5, 0b0'100'0000 },
        { Input::CH5_RELATIVE_TO_CH4, 0b0'101'0000 },
        { Input::CH6_RELATIVE_TO_CH7, 0b0'110'0000 },
        { Input::CH7_RELATIVE_TO_CH6, 0b0'111'0000 },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const in_sequence = InSequence{};

        auto controller             = Mock_Controller{};
        auto device_selector        = Mock_Device_Selector{};
        auto device_selector_handle = device_selector.handle();

        auto mcp3008 = Driver{ controller,
                               random<Mock_Controller::Configuration>(),
                               device_selector.handle() };

        auto const sample = random<Sample>();

        auto const tx = std::vector<std::uint8_t>{
            0x01,
            test_case.control_byte,
            0x00,
        };
        auto const rx = std::vector<std::uint8_t>{
            random<std::uint8_t>(),
            static_cast<std::uint8_t>(
                ( random<std::uint8_t>( 0b000000, 0b111111 ) << 2 )
                | ( sample.as_unsigned_integer() >> std::numeric_limits<std::uint8_t>::digits ) ),
            static_cast<std::uint8_t>( sample.as_unsigned_integer() ),
        };
        EXPECT_CALL( mcp3008, configure() );
        EXPECT_CALL( mcp3008, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
        EXPECT_CALL( device_selector, select() );
        EXPECT_CALL( mcp3008, exchange( tx ) ).WillOnce( Return( rx ) );
        EXPECT_CALL( device_selector, deselect() );

        EXPECT_EQ( mcp3008.sample( test_case.input ), sample );
    } // for
}

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
