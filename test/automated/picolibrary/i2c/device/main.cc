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
 * \brief picolibrary::I2C::Device automated test program.
 */

#include <cstdint>
#include <functional>
#include <limits>
#include <ostream>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/i2c.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/i2c.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::Response;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::testing::A;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

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

auto to_vector( std::uint16_t register_address ) -> std::vector<std::uint8_t>
{
    return { static_cast<std::uint8_t>( register_address >> std::numeric_limits<std::uint8_t>::digits ),
             static_cast<std::uint8_t>( register_address ) };
}

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
    auto const address                    = Address_Transmitted{ 0b1110010'0 };
    auto const nonresponsive_device_error = Mock_Error{ 12 };

    auto const device = Device{ {}, controller, address, nonresponsive_device_error };

    EXPECT_EQ( &device.controller(), &controller );
    EXPECT_EQ( device.address(), address );
    EXPECT_EQ( device.nonresponsive_device_error(), nonresponsive_device_error );
}

/**
 * \brief Verify picolibrary::I2C::Controller::align_bus_multiplexer() works properly.
 */
TEST( alignBusMultiplexer, worksProperly )
{
    auto bus_multiplexer_aligner = MockFunction<void()>{};
    auto controller              = Mock_Controller{};

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, 0b1110010'0, Mock_Error{ 190 }
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    device.align_bus_multiplexer();
}

/**
 * \brief picolibrary::I2C::Controller::ping( picolibrary::I2C::Operation ) test case.
 */
struct pingOperation_Test_Case {
    /**
     * \brief The operation to request when addressing the device.
     */
    Operation operation;

    /**
     * \brief The device's response.
     */
    Response response;
};

auto operator<<( std::ostream & stream, pingOperation_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".operation = " << test_case.operation
                  << ", "
                  << ".response = " << test_case.response
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::I2C::Controller::ping( picolibrary::I2C::Operation ) test fixture.
 */
class pingOperation : public TestWithParam<pingOperation_Test_Case> {
};

/**
 * \brief Verify picolibrary::I2C::Controller::ping( picolibrary::I2C::Operation ) works
 *        properly.
 */
TEST_P( pingOperation, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, test_case.operation ) )
        .WillOnce( Return( test_case.response ) );
    if ( test_case.operation == Operation::READ and test_case.response == Response::ACK ) {
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xCC ) );
    } // if
    EXPECT_CALL( controller, stop() );

    EXPECT_EQ( device.ping( test_case.operation ), test_case.response );
}

/**
 * \brief picolibrary::I2C::Controller::ping( picolibrary::I2C::Operation ) test cases.
 */
pingOperation_Test_Case const pingOperation_TEST_CASES[]{
    // clang-format off

    { Operation::READ,  Response::ACK  },
    { Operation::READ,  Response::NACK },
    { Operation::WRITE, Response::ACK  },
    { Operation::WRITE, Response::NACK },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pingOperation, ValuesIn( pingOperation_TEST_CASES ) );

/**
 * \brief picolibrary::I2C::Controller::ping() test case.
 */
struct ping_Test_Case {
    /**
     * \brief The device's response to the read request.
     */
    Response response_read;

    /**
     * \brief The device's response to the write request.
     */
    Response response_write;

    /**
     * \brief The device's response.
     */
    Response response;
};

auto operator<<( std::ostream & stream, ping_Test_Case const & test_case ) -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".response_read = " << test_case.response_read
                  << ", "
                  << ".response_write = " << test_case.response_write
                  << ", "
                  << ".response = " << test_case.response
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::I2C::Controller::ping() test fixture.
 */
class ping : public TestWithParam<ping_Test_Case> {
};

/**
 * \brief Verify picolibrary::I2C::Controller::ping() works properly.
 */
TEST_P( ping, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) )
        .WillOnce( Return( test_case.response_read ) );
    if ( test_case.response_read == Response::ACK ) {
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xCC ) );
    } // if
    EXPECT_CALL( controller, stop() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) )
        .WillOnce( Return( test_case.response_write ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_EQ( device.ping(), test_case.response );
}

/**
 * \brief picolibrary::I2C::Controller::ping() test cases.
 */
ping_Test_Case const ping_TEST_CASES[]{
    // clang-format off

    { Response::ACK,  Response::ACK,  Response::ACK  },
    { Response::ACK,  Response::NACK, Response::NACK },
    { Response::NACK, Response::ACK,  Response::NACK },
    { Response::NACK, Response::NACK, Response::NACK },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, ping, ValuesIn( ping_TEST_CASES ) );

/**
 * \brief Verify picolibrary::I2C::Device::read() works properly.
 */
TEST( readRegister0BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const data = std::uint8_t{ 0x82 };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( data ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_EQ( device.read(), data );
}

/**
 * \brief Verify picolibrary::I2C::Device::read( std::uint8_t *, std::uint8_t * ) works
 *        properly.
 */
TEST( readRegisterBlock0BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const data_expected = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, read( A<std::vector<std::uint8_t>>(), Response::NACK ) ).WillOnce( Return( data_expected ) );
    EXPECT_CALL( controller, stop() );

    auto data = std::vector<std::uint8_t>( data_expected.size() );

    device.read( &*data.begin(), &*data.end() );

    EXPECT_EQ( data, data_expected );
}

/**
 * \brief Verify picolibrary::I2C::Device::read( std::uint8_t ) works properly.
 */
TEST( readRegister8BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const register_address = std::uint8_t{ 0xB2 };
    auto const data             = std::uint8_t{ 0x82 };

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
TEST( readRegisterBlock8BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const register_address = std::uint8_t{ 0xB2 };
    auto const data_expected    = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

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
 * \brief Verify picolibrary::I2C::Device::read( std::uint16_t ) works properly.
 */
TEST( readRegister16BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const register_address = std::uint16_t{ 0x1F63 };
    auto const data             = std::uint8_t{ 0x82 };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( to_vector( register_address ) ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, repeated_start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( data ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_EQ( device.read( register_address ), data );
}

/**
 * \brief Verify picolibrary::I2C::Device::read( std::uint16_t, std::uint8_t *,
 *        std::uint8_t * ) works properly.
 */
TEST( readRegisterBlock16BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto const device = Device{
        bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 }
    };

    auto const register_address = std::uint16_t{ 0x1F63 };
    auto const data_expected    = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( to_vector( register_address ) ) ).WillOnce( Return( Response::ACK ) );
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
TEST( writeRegister0BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const data = std::uint8_t{ 0x82 };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( data );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint8_t const *, std::uint8_t const
 *        * ) works properly.
 */
TEST( writeRegisterBlock0BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const data = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( &*data.begin(), &*data.end() );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint8_t, std::uint8_t ) works
 *        properly.
 */
TEST( writeRegister8BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const register_address = std::uint8_t{ 0xB2 };
    auto const data             = std::uint8_t{ 0x82 };

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
TEST( writeRegisterBlock8BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const register_address = std::uint8_t{ 0xB2 };
    auto const data             = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( register_address ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( register_address, &*data.begin(), &*data.end() );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint16_t, std::uint8_t ) works
 *        properly.
 */
TEST( writeRegister16BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const register_address = std::uint16_t{ 0x1F63 };
    auto const data             = std::uint8_t{ 0x82 };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( to_vector( register_address ) ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( register_address, data );
}

/**
 * \brief Verify picolibrary::I2C::Device::write( std::uint16_t, std::uint8_t const *,
 *        std::uint8_t const * ) works properly.
 */
TEST( writeRegisterBlock16BitRegisterAddress, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto       bus_multiplexer_aligner = MockFunction<void()>{};
    auto       controller              = Mock_Controller{};
    auto const address                 = Address_Transmitted{ 0b1110010'0 };

    auto device = Device{ bus_multiplexer_aligner.AsStdFunction(), controller, address, Mock_Error{ 32 } };

    auto const register_address = std::uint16_t{ 0x1F63 };
    auto const data             = std::vector<std::uint8_t>{
        0x85, 0xA3, 0x98, 0xC7, 0x39, 0x6A, 0xFA, 0xC5, 0xA4,
    };

    EXPECT_CALL( bus_multiplexer_aligner, Call() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( to_vector( register_address ) ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, write( data ) ).WillOnce( Return( Response::ACK ) );
    EXPECT_CALL( controller, stop() );

    device.write( register_address, &*data.begin(), &*data.end() );
}

/**
 * \brief Execute the picolibrary::I2C::Device automated tests.
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
