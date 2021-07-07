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
 * \brief picolibrary::WIZnet::W5500::Communication_Controller unit test program.
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::picolibrary::WIZnet::W5500::Operation;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::ReturnRef;

class Communication_Controller :
    public ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device> {
  public:
    Communication_Controller()
    {
    }

    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::read;
};

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) works properly.
 */
TEST( readCommonRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const offset = random<std::uint16_t>();
    auto const frame  = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>( Operation::READ ),
    };
    auto const data = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    ;
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read( offset );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Execute the picolibrary::WIZnet::W5500::Communication_Controller unit tests.
 *
 * \param[in] argc The number of arguments to pass to testing::InitGoogleMock().
 * \param[in] argv The array  of arguments to pass to testing::InitGoogleMock().
 *
 * \return See Google Test's RUN_ALL_TESTS().
 */
int main( int argc, char * argv[] )
{
    ::testing::InitGoogleMock( &argc, argv );

    return RUN_ALL_TESTS();
}
