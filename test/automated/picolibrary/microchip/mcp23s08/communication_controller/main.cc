/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Microchip::MCP23S08::Communication_Controller automated test
 *        program.
 */

#include <cstdint>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/microchip/mcp23s08.h"
#include "picolibrary/testing/automated/spi.h"

namespace {

using ::picolibrary::Microchip::MCP23S08::Address_Numeric;
using ::picolibrary::Microchip::MCP23S08::Address_Transmitted;
using ::picolibrary::Testing::Automated::SPI::Mock_Controller;
using ::picolibrary::Testing::Automated::SPI::Mock_Device;
using ::picolibrary::Testing::Automated::SPI::Mock_Device_Selector;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

class Communication_Controller :
    public ::picolibrary::Microchip::MCP23S08::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device> {
  public:
    Communication_Controller(
        Mock_Controller &                      controller,
        Mock_Controller::Configuration const & configuration,
        Mock_Device_Selector::Handle           device_selector,
        Address_Transmitted                    address ) noexcept :
        ::picolibrary::Microchip::MCP23S08::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>{
            controller,
            configuration,
            std::move( device_selector ),
            std::move( address )
        }
    {
    }

    using ::picolibrary::Microchip::MCP23S08::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::read;

    using ::picolibrary::Microchip::MCP23S08::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::write;
};

} // namespace

/**
 * \brief picolibrary::Microchip::MCP23S08::Communication_Controller::Communication_Controller(
 *        Controller &, Controller::Configuration const &, Device_Selector,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test fixture.
 */
class constructor : public TestWithParam<Address_Transmitted> {
};

/**
 * \brief Verify
 *        picolibrary::Microchip::MCP23S08::Communication_Controller::Communication_Controller(
 *        Controller &, Controller::Configuration const &, Device_Selector,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) works properly.
 */
TEST_P( constructor, worksProperly )
{
    auto       controller      = Mock_Controller{};
    auto       device_selector = Mock_Device_Selector{};
    auto const address         = GetParam();

    auto const communication_controller = Communication_Controller{
        controller, 95, device_selector.handle(), address
    };

    EXPECT_EQ( communication_controller.address(), address );
}

/**
 * \brief picolibrary::Microchip::MCP23S08::Communication_Controller::Communication_Controller(
 *        Controller &, Controller::Configuration const &, Device_Selector,
 *        picolibrary::Microchip::MCP23S08::Address_Transmitted ) test cases.
 */
Address_Transmitted const constructor_TEST_CASES[]{
    // clang-format off

    { Address_Numeric{ 0b01000'00 } },
    { Address_Numeric{ 0b01000'01 } },
    { Address_Numeric{ 0b01000'10 } },
    { Address_Numeric{ 0b01000'11 } },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, constructor, ValuesIn( constructor_TEST_CASES ) );

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Communication_Controller::read() works
 *        properly.
 */
TEST( read, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       controller             = Mock_Controller{};
    auto       device_selector        = Mock_Device_Selector{};
    auto       device_selector_handle = device_selector.handle();
    auto const address = Address_Transmitted{ Address_Numeric{ 0b01000'01 } };

    auto const communication_controller = Communication_Controller{
        controller, 95, device_selector.handle(), address
    };

    auto const register_address = std::uint8_t{ 0x67 };
    auto const data             = std::uint8_t{ 0xA5 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( address.as_unsigned_integer() | 0b1 ) );
    EXPECT_CALL( communication_controller, transmit( register_address ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() );

    EXPECT_EQ( communication_controller.read( register_address ), data );
}

/**
 * \brief Verify picolibrary::Microchip::MCP23S08::Communication_Controller::write() works
 *        properly.
 */
TEST( write, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       controller             = Mock_Controller{};
    auto       device_selector        = Mock_Device_Selector{};
    auto       device_selector_handle = device_selector.handle();
    auto const address = Address_Transmitted{ Address_Numeric{ 0b01000'10 } };

    auto communication_controller = Communication_Controller{
        controller, 95, device_selector.handle(), address
    };

    auto const register_address = std::uint8_t{ 0x97 };
    auto const data             = std::uint8_t{ 0x56 };

    EXPECT_CALL( communication_controller, configure() );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() );
    EXPECT_CALL( communication_controller, transmit( address.as_unsigned_integer() ) );
    EXPECT_CALL( communication_controller, transmit( register_address ) );
    EXPECT_CALL( communication_controller, transmit( data ) );
    EXPECT_CALL( device_selector, deselect() );

    communication_controller.write( register_address, data );
}

/**
 * \brief Execute the picolibrary::Microchip::MCP23S08::Communication_Controller automated
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
