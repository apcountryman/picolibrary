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
 * \brief picolibrary::I2C::Device<std::uint8_t, Controller, Bus_Multiplexer_Aligner> unit
 *        test program.
 */

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::I2C::Address;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::Response;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::testing::_;
using ::testing::A;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Return;

class Device :
    public ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>> {
  public:
    Device(
        std::function<Result<Void, Error_Code>()> bus_multiplexer_aligner,
        Mock_Controller &                         controller,
        Address                                   address,
        Error_Code const &                        nonresponsive_device_error ) noexcept :
        ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>{
            std::move( bus_multiplexer_aligner ),
            controller,
            address,
            nonresponsive_device_error
        }
    {
    }

    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::change_address;
    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::align_bus_multiplexer;
    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::controller;
    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::read;
    using ::picolibrary::I2C::Device<std::uint8_t, Mock_Controller, std::function<Result<Void, Error_Code>()>>::write;
};

} // namespace

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::Device( Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::I2C::Address, picolibrary::Error_Code const & ) works properly.
 */
TEST( constructor, worksProperly )
{
    auto       controller                 = Mock_Controller{};
    auto const address                    = random<Address>();
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{ {}, controller, address, nonresponsive_device_error };

    EXPECT_EQ( &device.controller(), &controller );
    EXPECT_EQ( device.address(), address );
    EXPECT_EQ( device.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::change_address() works properly.
 */
TEST( changeAddress, worksProperly )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const address = random<Address>();

    device.change_address( address );

    EXPECT_EQ( device.address(), address );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::align_bus_multiplexer() properly handles a bus
 *        multiplexer alignment error.
 */
TEST( alignBusMultiplexer, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const result = device.align_bus_multiplexer();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::align_bus_multiplexer() works properly.
 */
TEST( alignBusMultiplexer, worksProperly )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.align_bus_multiplexer().is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles a
 *        bus multiplexer alignment error.
 */
TEST( pingOperation, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const result = device.ping( random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles a
 *        start condition transmission error.
 */
TEST( pingOperation, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = device.ping( random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles
 *        an addressing error.
 */
TEST( pingOperation, addressingError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping( random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles a
 *        nonresponsive device error.
 */
TEST( pingOperation, nonresponsiveDeviceError )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping( random<Operation>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles a
 *        read error.
 */
TEST( pingOperation, readError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping( Operation::READ );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) properly handles a
 *        stop condition transmission error.
 */
TEST( pingOperation, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );

    EXPECT_FALSE( device.ping( random<Operation>() ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping( picolibrary::I2C::Operation ) works properly.
 */
TEST( pingOperation, worksProperly )
{
    {
        auto const in_sequence = InSequence{};

        auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
        auto       controller              = Mock_Controller{};
        auto const address                 = random<Address>();

        auto const device = Device{
            bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
        };

        EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, address( address, Operation::READ ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( device.ping( Operation::READ ).is_error() );
    }

    {
        auto const in_sequence = InSequence{};

        auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
        auto       controller              = Mock_Controller{};
        auto const address                 = random<Address>();

        auto const device = Device{
            bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
        };

        EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, address( address, Operation::WRITE ) )
            .WillOnce( Return( Result<Void, Error_Code>{} ) );
        EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

        EXPECT_FALSE( device.ping( Operation::WRITE ).is_error() );
    }
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles a bus multiplexer alignment
 *        error.
 */
TEST( ping, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const result = device.ping();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles a start condition transmission
 *        error.
 */
TEST( ping, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = device.ping();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles an addressing error.
 */
TEST( ping, addressingError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles a nonresponsive device error.
 */
TEST( ping, nonresponsiveDeviceError )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles a read error.
 */
TEST( ping, readError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.ping();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() properly handles a stop condition transmission
 *        error.
 */
TEST( ping, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillRepeatedly( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( controller, stop() ).WillRepeatedly( Return( random<Mock_Error>() ) );

    EXPECT_FALSE( device.ping().is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::ping() works properly.
 */
TEST( ping, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.ping().is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a bus
 *        multiplexer alignment error.
 */
TEST( readRegister, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a start
 *        condition transmission error.
 */
TEST( readRegister, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles an addressing
 *        error when preparing to write the register address.
 */
TEST( readRegister, addressingErrorAddressWrite )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a nonresponsive
 *        device error when preparing to write the register address.
 */
TEST( readRegister, nonresponsiveDeviceErrorAddressWrite )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a write error.
 */
TEST( readRegister, writeError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a nonresponsive
 *        device error when writing the register address.
 */
TEST( readRegister, nonresponsiveDeviceErrorWriteRegisterAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a repeated start
 *        condition transmission error.
 */
TEST( readRegister, repeatedStartError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles an addressing
 *        error when preparing to read the register.
 */
TEST( readRegister, addressingErrorAddressRead )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a nonresponsive
 *        device error when preparing to read the register.
 */
TEST( readRegister, nonresponsiveDeviceErrorAddressRead )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a read error.
 */
TEST( readRegister, readError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) properly handles a stop condition
 *        transmission error.
 */
TEST( readRegister, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( _ ) ).WillOnce( Return( random<std::uint8_t>() ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );

    EXPECT_FALSE( device.read( random<std::uint8_t>() ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t ) works properly.
 */
TEST( readRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random<std::uint8_t>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( data ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.read( register_address );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), data );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a bus multiplexer alignment error.
 */
TEST( readRegisterBlock, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a start condition transmission error.
 */
TEST( readRegisterBlock, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles an addressing error when preparing to write the register block
 *        address.
 */
TEST( readRegisterBlock, addressingErrorAddressWrite )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a nonresponsive device error when preparing to write the
 *        register block address.
 */
TEST( readRegisterBlock, nonresponsiveDeviceErrorAddressWrite )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a write error.
 */
TEST( readRegisterBlock, writeError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a nonresponsive device error when writing the register block
 *        address.
 */
TEST( readRegisterBlock, nonresponsiveDeviceErrorWriteRegisterAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a repeated start condition transmission error.
 */
TEST( readRegisterBlock, repeatedStartError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles an addressing error when preparing to read the block of
 *        registers.
 */
TEST( readRegisterBlock, addressingErrorAddressRead )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a nonresponsive device error when preparing to read the block
 *        of registers.
 */
TEST( readRegisterBlock, nonresponsiveDeviceErrorAddressRead )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a read error.
 */
TEST( readRegisterBlock, readError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( A<std::vector<std::uint8_t>>(), _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto       data   = std::vector<std::uint8_t>( random<std::uint_fast8_t>( 1 ) );
    auto const result = device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        properly handles a stop condition transmission error.
 */
TEST( readRegisterBlock, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address>(),
                                random<Mock_Error>() };

    auto const size = random<std::uint_fast8_t>( 1 );

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( A<std::vector<std::uint8_t>>(), _ ) )
        .WillOnce( Return( random_container<std::vector<std::uint8_t>>( size ) ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );

    auto data = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( device.read( random<std::uint8_t>(), &*data.begin(), &*data.end() ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::read( std::uint8_t, std::uint8_t *, std::uint8_t * )
 *        works properly.
 */
TEST( readRegisterBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const size             = random<std::uint_fast8_t>();
    auto const data_expected    = random_container<std::vector<std::uint8_t>>( size );

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, repeated_start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, read( A<std::vector<std::uint8_t>>(), Response::NACK ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto data = std::vector<std::uint8_t>( size );
    EXPECT_FALSE( device.read( register_address, &*data.begin(), &*data.end() ).is_error() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        bus multiplexer alignment error.
 */
TEST( writeRegister, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        start condition transmission error.
 */
TEST( writeRegister, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles
 *        an addressing error.
 */
TEST( writeRegister, addressingError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        nonresponsive device error when addressing the device.
 */
TEST( writeRegister, nonresponsiveDeviceErrorAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        write error when writing the register address.
 */
TEST( writeRegister, writeErrorWriteRegisterAddress )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        nonresponsive device error when writing the register address.
 */
TEST( writeRegister, nonresponsiveDeviceErrorWriteRegisterAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        write error when writing the data.
 */
TEST( writeRegister, writeErrorWriteData )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        nonresponsive device error when writing the data.
 */
TEST( writeRegister, nonresponsiveDeviceErrorWriteData )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) )
        .WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const result = device.write( random<std::uint8_t>(), random<std::uint8_t>() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) properly handles a
 *        stop condition transmission error.
 */
TEST( writeRegister, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillRepeatedly( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );

    EXPECT_FALSE( device.write( random<std::uint8_t>(), random<std::uint8_t>() ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t ) works properly.
 */
TEST( writeRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random<std::uint8_t>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.write( register_address, data ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a bus multiplexer alignment error.
 */
TEST( writeRegisterBlock, alignmentError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a start condition transmission error.
 */
TEST( writeRegisterBlock, startError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( error ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles an addressing error.
 */
TEST( writeRegisterBlock, addressingError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a nonresponsive device error when
 *        addressing the device.
 */
TEST( writeRegisterBlock, nonresponsiveDeviceErrorAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a write error when writing the register
 *        block address.
 */
TEST( writeRegisterBlock, writeErrorWriteRegisterAddress )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a nonresponsive device error when
 *        writing the register block address.
 */
TEST( writeRegisterBlock, nonresponsiveDeviceErrorWriteRegisterAddress )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a write error when writing the data.
 */
TEST( writeRegisterBlock, writeErrorWriteData )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    auto const error = random<Mock_Error>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a nonresponsive device error when
 *        writing the data.
 */
TEST( writeRegisterBlock, nonresponsiveDeviceErrorWriteData )
{
    auto       bus_multiplexer_aligner    = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller                 = Mock_Controller{};
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, random<Address>(), nonresponsive_device_error
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( Generic_Error::NONRESPONSIVE_DEVICE ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) properly handles a stop condition transmission error.
 */
TEST( writeRegisterBlock, stopError )
{
    auto bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto controller              = Mock_Controller{};

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                          controller,
                          random<Address>(),
                          random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::uint8_t>() ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( A<std::vector<std::uint8_t>>() ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( random<Mock_Error>() ) );

    auto const data = random_container<std::vector<std::uint8_t>>();
    auto const result = device.write( random<std::uint8_t>(), &*data.begin(), &*data.end() );
}

/**
 * \brief Verify picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner>::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( writeRegisterBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<Result<Void, Error_Code>()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, start() ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );
    EXPECT_CALL( controller, stop() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( device.write( register_address, &*data.begin(), &*data.end() ).is_error() );
}

/**
 * \brief Execute the picolibrary::I2C::Device<std::uint8_t, Controller,
 *        Bus_Multiplexer_Aligner> unit tests.
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
