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
 * \brief picolibrary::I2C::Device unit test program.
 */

#include <cstdint>
#include <functional>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/i2c.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::Response;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::I2C::Mock_Controller;
using ::testing::A;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Return;

class Device : public ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller> {
  public:
    Device() noexcept = default;

    Device(
        std::function<void()> bus_multiplexer_aligner,
        Mock_Controller &     controller,
        Address_Transmitted   address,
        Error_Code const &    nonresponsive_device_error ) noexcept :
        ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller>{ std::move( bus_multiplexer_aligner ),
                                                                            controller,
                                                                            std::move( address ),
                                                                            nonresponsive_device_error }
    {
    }

    Device( Device && source ) noexcept = default;

    Device( Device const & ) = delete;

    ~Device() noexcept = default;

    auto operator=( Device && expression ) noexcept -> Device & = default;

    auto operator=( Device const & ) = delete;

    using ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller>::align_bus_multiplexer;

    using ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller>::controller;

    using ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller>::read;

    using ::picolibrary::I2C::Device<std::function<void()>, Mock_Controller>::write;
};

} // namespace

/**
 * \brief Verify picolibrary::I2C::Device::Device() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const device = Device{};

    EXPECT_EQ( &device.controller(), nullptr );
    EXPECT_EQ( device.address(), Address_Transmitted{} );
    EXPECT_EQ( device.nonresponsive_device_error(), Error_Code{} );
}

/**
 * \brief Verify picolibrary::I2C::Device::Device( Bus_Multiplexer_Aligner, Controller &,
 *        picolibrary::I2C::Address_Transmitted, picolibrary::Error_Code const & ) works
 *        properly.
 */
TEST( constructor, worksProperly )
{
    auto       controller                 = Mock_Controller{};
    auto const address                    = random<Address_Transmitted>();
    auto const nonresponsive_device_error = random<Mock_Error>();

    auto const device = Device{ {}, controller, address, nonresponsive_device_error };

    EXPECT_EQ( &device.controller(), &controller );
    EXPECT_EQ( device.address(), address );
    EXPECT_EQ( device.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Device::align_bus_multiplexer() works properly.
 */
TEST( alignBusMultiplexer, worksProperly )
{
    auto bus_multiplexer_aligner = MockFunction<void()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                controller,
                                random<Address_Transmitted>(),
                                random<Mock_Error>() };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    device.align_bus_multiplexer();
}

/**
 * \brief Verify picolibrary::I2C::Device::ping() works properly.
 */
TEST( ping, worksProperly )
{
    {
        struct {
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK  },
            { Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto       bus_multiplexer_aligner = MockFunction<void()>{};
            auto       controller              = Mock_Controller{};
            auto const address                 = random<Address_Transmitted>();

            auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                        controller,
                                        address,
                                        random<Mock_Error>() };

            EXPECT_CALL( bus_multiplexer_aligner, Call() );
            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::READ ) )
                .WillOnce( Return( test_case.response ) );
            if ( test_case.response == Response::ACK ) {
                EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            } // if
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( device.ping( Operation::READ ), test_case.response );
        } // for
    }

    {
        struct {
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK  },
            { Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto       bus_multiplexer_aligner = MockFunction<void()>{};
            auto       controller              = Mock_Controller{};
            auto const address                 = random<Address_Transmitted>();

            auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                        controller,
                                        address,
                                        random<Mock_Error>() };

            EXPECT_CALL( bus_multiplexer_aligner, Call() );
            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::WRITE ) )
                .WillOnce( Return( test_case.response ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( device.ping( Operation::WRITE ), test_case.response );
        } // for
    }

    {
        struct {
            Response response_read;
            Response response_write;
            Response response;
        } const test_cases[]{
            // clang-format off

            { Response::ACK,  Response::ACK,  Response::ACK  },
            { Response::ACK,  Response::NACK, Response::NACK },
            { Response::NACK, Response::ACK,  Response::NACK },
            { Response::NACK, Response::NACK, Response::NACK },

            // clang-format on
        };

        for ( auto const test_case : test_cases ) {
            auto const in_sequence = InSequence{};

            auto       bus_multiplexer_aligner = MockFunction<void()>{};
            auto       controller              = Mock_Controller{};
            auto const address                 = random<Address_Transmitted>();

            auto const device = Device{ bus_multiplexer_aligner.AsStdFunction(),
                                        controller,
                                        address,
                                        random<Mock_Error>() };

            EXPECT_CALL( bus_multiplexer_aligner, Call() );
            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::READ ) )
                .WillOnce( Return( test_case.response_read ) );
            if ( test_case.response_read == Response::ACK ) {
                EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( random<std::uint8_t>() ) );
            } // if
            EXPECT_CALL( controller, stop() );

            EXPECT_CALL( controller, start() );
            EXPECT_CALL( controller, address( address, Operation::WRITE ) )
                .WillOnce( Return( test_case.response_write ) );
            EXPECT_CALL( controller, stop() );

            EXPECT_EQ( device.ping(), test_case.response );
        } // for
    }
}

/**
 * \brief Verify picolibrary::I2C::Device::read( std::uint8_t ) works properly.
 */
TEST( readRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address_Transmitted>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random<std::uint8_t>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() );
    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, repeated_start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( data ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_EQ( device.read( register_address ), data );
}

/**
 * \brief Verify picolibrary::I2C::Device::read( std::uint8_t, std::uint8_t *,
 *        std::uint8_t * ) works properly.
 */
TEST( readRegisterBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address_Transmitted>();

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data_expected    = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() );
    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, repeated_start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, read( A<std::vector<std::uint8_t>>(), Response::NACK ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( controller, stop() );

    auto data = std::vector<std::uint8_t>( data_expected.size() );
    device.read( register_address, &*data.begin(), &*data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint8_t ) works properly.
 */
TEST( writeRegister, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address_Transmitted>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random<std::uint8_t>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() );
    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( register_address, data );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint8_t, std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( writeRegisterBlock, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = random<Address_Transmitted>();

    auto device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, random<Mock_Error>()
    };

    auto const register_address = random<std::uint8_t>();
    auto const data             = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( bus_multiplexer_aligner, Call() );
    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( register_address, &*data.begin(), &*data.end() );
}

/**
 * \brief Execute the picolibrary::I2C::Device unit tests.
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
