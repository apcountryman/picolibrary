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
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/testing/unit/wiznet/w5500.h"
#include "picolibrary/void.h"
#include "picolibrary/wiznet/w5500.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::SPI::Mock_Controller;
using ::picolibrary::Testing::Unit::SPI::Mock_Device;
using ::picolibrary::Testing::Unit::SPI::Mock_Device_Selector;
using ::picolibrary::WIZnet::W5500::Operation;
using ::picolibrary::WIZnet::W5500::Region;
using ::picolibrary::WIZnet::W5500::Socket_ID;
using ::picolibrary::WIZnet::W5500::SPI_Mode;
using ::testing::_;
using ::testing::A;
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
    using ::picolibrary::WIZnet::W5500::Communication_Controller<Mock_Controller, Mock_Device_Selector::Handle, Mock_Device>::write;
};

} // namespace

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) properly handles a configuration error.
 */
TEST( readCommonRegisterMemory, configurationError )
{
    auto const communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto const result = communication_controller.read( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) properly handles a selection error.
 */
TEST( readCommonRegisterMemory, selectionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto const result = communication_controller.read( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) properly handles a frame transmission error.
 */
TEST( readCommonRegisterMemory, frameTransmissionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) properly handles a data reception error.
 */
TEST( readCommonRegisterMemory, dataReceptionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read( random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read( std::uint16_t
 *        ) works properly.
 */
TEST( readCommonRegisterMemory, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const offset = random<std::uint16_t>();
    auto const frame  = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( Operation::READ ),
    };
    auto const data = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read( offset );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a configuration
 *        error.
 */
TEST( readCommonRegisterMemoryBlock, configurationError )
{
    auto const communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a selection
 *        error.
 */
TEST( readCommonRegisterMemoryBlock, selectionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a frame
 *        transmission error.
 */
TEST( readCommonRegisterMemoryBlock, frameTransmissionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a data
 *        reception error.
 */
TEST( readCommonRegisterMemoryBlock, dataReceptionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) works properly.
 */
TEST( readCommonRegisterMemoryBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const offset = random<std::uint16_t>();
    auto const frame  = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( Operation::READ ),
    };
    auto const size          = random<std::uint_fast8_t>();
    auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive( _ ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto data = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( communication_controller.read( offset, &*data.begin(), &*data.end() ).is_error() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t ) properly handles a configuration error.
 */
TEST( writeCommonRegister, configurationError )
{
    auto communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto const result = communication_controller.write(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t ) properly handles a selection error.
 */
TEST( writeCommonRegister, selectionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto const result = communication_controller.write(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        std::uint16_t, std::uint8_t ) properly handles a frame transmission error.
 */
TEST( writeCommonRegister, frameTransmissionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.write(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t ) properly handles a data reception error.
 */
TEST( writeCommonRegister, dataTransmissionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.write(
        random<std::uint16_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t ) works properly.
 */
TEST( writeCommonRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const offset = random<std::uint16_t>();
    auto const frame  = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( Operation::WRITE ),
    };
    auto const data = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( communication_controller.write( offset, data ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t const *, std::uint8_t const * ) properly handles a
 *        configuration error.
 */
TEST( writeCommonRegisterBlock, configurationError )
{
    auto communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = communication_controller.write(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t const *, std::uint8_t const * ) properly handles a
 *        selection error.
 */
TEST( writeCommonRegisterBlock, selectionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = communication_controller.write(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t const *, std::uint8_t const * ) properly handles a
 *        frame transmission error.
 */
TEST( writeCommonRegisterBlock, frameTransmissionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = communication_controller.write(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t const *, std::uint8_t const * ) properly handles a
 *        data transmission error.
 */
TEST( writeCommonRegisterBlock, dataTransmissionError )
{
    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data   = random_container<std::vector<std::uint8_t>>();
    auto const result = communication_controller.write(
        random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::write(
 *        std::uint16_t, std::uint8_t const *, std::uint8_t const * ) works properly.
 */
TEST( writeCommonRegisterBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const offset = random<std::uint16_t>();
    auto const frame  = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( Operation::WRITE ),
    };
    auto const data = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( communication_controller.write( offset, &*data.begin(), &*data.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t ) properly handles a configuration error.
 */
TEST( readSocketRegisterBufferMemory, configurationError )
{
    auto const communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t ) properly handles a selection error.
 */
TEST( readSocketRegisterBufferMemory, selectionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t ) properly handles a frame transmission error.
 */
TEST( readSocketRegisterBufferMemory, frameTransmissionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t ) properly handles a data reception error.
 */
TEST( readSocketRegisterBufferMemory, dataReceptionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t ) works properly.
 */
TEST( readSocketRegisterBufferMemory, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const socket_id = random<Socket_ID>();
    auto const region    = random<Region>();
    auto const offset    = random<std::uint16_t>();
    auto const frame     = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>(
            static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( socket_id ) | static_cast<std::uint8_t>( region )
            | static_cast<std::uint8_t>( Operation::READ ) ),
    };
    auto const data = random<std::uint8_t>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive() ).WillOnce( Return( data ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = communication_controller.read( socket_id, region, offset );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a configuration
 *        error.
 */
TEST( readSocketRegisterBufferMemoryBlock, configurationError )
{
    auto const communication_controller = Communication_Controller{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a selection
 *        error.
 */
TEST( readSocketRegisterBufferMemoryBlock, selectionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a frame
 *        transmission error.
 */
TEST( readSocketRegisterBufferMemoryBlock, frameTransmissionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) properly handles a data
 *        reception error.
 */
TEST( readSocketRegisterBufferMemoryBlock, dataReceptionError )
{
    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = communication_controller.read(
        random<Socket_ID>(), random<Region>(), random<std::uint16_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::WIZnet::W5500::Communication_Controller::read(
 *        picolibrary::WIZnet::W5500::Socket_ID, picolibrary::WIZnet::W5500::Region,
 *        std::uint16_t, std::uint8_t *, std::uint8_t * ) works properly.
 */
TEST( readSocketRegisterBufferMemoryBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto const communication_controller = Communication_Controller{};

    auto device_selector        = Mock_Device_Selector{};
    auto device_selector_handle = device_selector.handle();

    auto const socket_id = random<Socket_ID>();
    auto const region    = random<Region>();
    auto const offset    = random<std::uint16_t>();
    auto const frame     = std::vector<std::uint8_t>{
        static_cast<std::uint8_t>( offset >> std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( offset ),
        static_cast<std::uint8_t>(
            static_cast<std::uint8_t>( SPI_Mode::VARIABLE_LENGTH_DATA )
            | static_cast<std::uint8_t>( socket_id ) | static_cast<std::uint8_t>( region )
            | static_cast<std::uint8_t>( Operation::READ ) ),
    };
    auto const size          = random<std::uint_fast8_t>();
    auto const data_expected = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( communication_controller, configure() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, device_selector() ).WillOnce( ReturnRef( device_selector_handle ) );
    EXPECT_CALL( device_selector, select() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, transmit( frame ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( communication_controller, receive( _ ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( device_selector, deselect() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto data = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( communication_controller
                      .read( socket_id, region, offset, &*data.begin(), &*data.end() )
                      .is_error() );

    EXPECT_EQ( data, data_expected );
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
