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
 * \brief picolibrary::I2C automated test program.
 */

#include <ostream>
#include <type_traits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/algorithm.h"
#include "picolibrary/i2c.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/i2c.h"

namespace {

using ::picolibrary::Functor_Can_Fail_Discard_Functor;
using ::picolibrary::Functor_Can_Fail_Return_Functor;
using ::picolibrary::Result;
using ::picolibrary::I2C::Address_Transmitted;
using ::picolibrary::I2C::Operation;
using ::picolibrary::I2C::Response;
using ::picolibrary::I2C::scan;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::I2C::Mock_Controller;
using ::testing::_;
using ::testing::InSequence;
using ::testing::MockFunction;
using ::testing::Return;
using ::testing::TestWithParam;
using ::testing::ValuesIn;

auto as_string( Operation operation ) -> char const *
{
    switch ( operation ) {
        case Operation::READ: return "Operation::READ";
        case Operation::WRITE: return "Operation::WRITE";
        default: return "UNKNOWN";
    } // switch
}

auto as_string( Response response ) -> char const *
{
    switch ( response ) {
        case Response::ACK: return "Response::ACK";
        case Response::NACK: return "Response::NACK";
        default: return "UNKNOWN";
    } // switch
}

} // namespace

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Operation ) test case.
 */
struct pingControllerI2CAddressTransmittedI2COperation_Test_Case {
    /**
     * \brief The operation to request when addressing the device.
     */
    Operation operation;

    /**
     * \brief The device's response.
     */
    Response response;
};

auto operator<<( std::ostream & stream, pingControllerI2CAddressTransmittedI2COperation_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".operation = " << as_string( test_case.operation )
                  << ", "
                  << ".response = " << as_string( test_case.response )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Operation ) test fixture.
 */
class pingControllerI2CAddressTransmittedI2COperation :
    public TestWithParam<pingControllerI2CAddressTransmittedI2COperation_Test_Case> {
};

/**
 * \brief Verify picolibrary::I2C::ping( Controller &,
 *        picolibrary::I2C::Address_Transmitted, picolibrary::I2C::Operation ) works
 *        properly.
 */
TEST_P( pingControllerI2CAddressTransmittedI2COperation, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};

    auto const address = Address_Transmitted{ 0b1000110'0 };

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, test_case.operation ) )
        .WillOnce( Return( test_case.response ) );
    if ( test_case.operation == Operation::READ and test_case.response == Response::ACK ) {
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xE0 ) );
    } // if
    EXPECT_CALL( controller, stop() );

    ASSERT_EQ( ping( controller, address, test_case.operation ), test_case.response );
}

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted,
 *        picolibrary::I2C::Operation ) test cases.
 */
pingControllerI2CAddressTransmittedI2COperation_Test_Case const pingControllerI2CAddressTransmittedI2COperation_TEST_CASES[]{
    // clang-format off

    { Operation::READ,  Response::ACK  },
    { Operation::READ,  Response::NACK },
    { Operation::WRITE, Response::ACK  },
    { Operation::WRITE, Response::NACK },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P(
    testCases,
    pingControllerI2CAddressTransmittedI2COperation,
    ValuesIn( pingControllerI2CAddressTransmittedI2COperation_TEST_CASES ) );

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted )
 *        test case.
 */
struct pingControllerI2CAddressTransmitted_Test_Case {
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

auto operator<<( std::ostream & stream, pingControllerI2CAddressTransmitted_Test_Case const & test_case )
    -> std::ostream &
{
    // clang-format off

    return stream << "{ "
                  << ".response_read = " << as_string( test_case.response_read )
                  << ", "
                  << ".response_write = " << as_string( test_case.response_write )
                  << ", "
                  << ".response = " << as_string( test_case.response )
                  << " }";

    // clang-format on
}

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted )
 *        test fixture.
 */
class pingControllerI2CAddressTransmitted :
    public TestWithParam<pingControllerI2CAddressTransmitted_Test_Case> {
};

/**
 * \brief Verify picolibrary::I2C::ping( Controller &,
 *        picolibrary::I2C::Address_Transmitted ) works properly.
 */
TEST_P( pingControllerI2CAddressTransmitted, worksProperly )
{
    auto const test_case = GetParam();

    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};

    auto const address = Address_Transmitted{ 0b1000110'0 };

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::READ ) )
        .WillOnce( Return( test_case.response_read ) );
    if ( test_case.response_read == Response::ACK ) {
        EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xE0 ) );
    } // if
    EXPECT_CALL( controller, stop() );

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( address, Operation::WRITE ) )
        .WillOnce( Return( test_case.response_write ) );
    EXPECT_CALL( controller, stop() );

    ASSERT_EQ( ping( controller, address ), test_case.response );
}

/**
 * \brief picolibrary::I2C::ping( Controller &, picolibrary::I2C::Address_Transmitted )
 *        test cases.
 */
pingControllerI2CAddressTransmitted_Test_Case const pingControllerI2CAddressTransmitted_TEST_CASES[]{
    // clang-format off

    { Response::ACK,  Response::ACK,  Response::ACK  },
    { Response::ACK,  Response::NACK, Response::NACK },
    { Response::NACK, Response::ACK,  Response::NACK },
    { Response::NACK, Response::NACK, Response::NACK },

    // clang-format on
};

INSTANTIATE_TEST_SUITE_P( testCases, pingControllerI2CAddressTransmitted, ValuesIn( pingControllerI2CAddressTransmitted_TEST_CASES ) );

/**
 * \brief picolibrary::I2C::scan() devices.
 */
struct {
    Address_Transmitted address;
    Response            response_read;
    Response            response_write;
} const scan_DEVICES[]{
    // clang-format off

    { 0x00 << 1, Response::ACK,  Response::NACK },
    { 0x01 << 1, Response::NACK, Response::ACK  },
    { 0x02 << 1, Response::ACK,  Response::NACK },
    { 0x03 << 1, Response::ACK,  Response::NACK },
    { 0x04 << 1, Response::ACK,  Response::ACK  },
    { 0x05 << 1, Response::NACK, Response::ACK  },
    { 0x06 << 1, Response::NACK, Response::ACK  },
    { 0x07 << 1, Response::ACK,  Response::NACK },
    { 0x08 << 1, Response::NACK, Response::ACK  },
    { 0x09 << 1, Response::NACK, Response::NACK },
    { 0x0A << 1, Response::ACK,  Response::NACK },
    { 0x0B << 1, Response::NACK, Response::NACK },
    { 0x0C << 1, Response::ACK,  Response::ACK  },
    { 0x0D << 1, Response::ACK,  Response::ACK  },
    { 0x0E << 1, Response::NACK, Response::ACK  },
    { 0x0F << 1, Response::ACK,  Response::NACK },

    { 0x10 << 1, Response::ACK,  Response::ACK  },
    { 0x11 << 1, Response::ACK,  Response::ACK  },
    { 0x12 << 1, Response::ACK,  Response::NACK },
    { 0x13 << 1, Response::NACK, Response::NACK },
    { 0x14 << 1, Response::ACK,  Response::ACK  },
    { 0x15 << 1, Response::NACK, Response::ACK  },
    { 0x16 << 1, Response::NACK, Response::ACK  },
    { 0x17 << 1, Response::ACK,  Response::NACK },
    { 0x18 << 1, Response::ACK,  Response::ACK  },
    { 0x19 << 1, Response::ACK,  Response::NACK },
    { 0x1A << 1, Response::NACK, Response::ACK  },
    { 0x1B << 1, Response::NACK, Response::NACK },
    { 0x1C << 1, Response::NACK, Response::NACK },
    { 0x1D << 1, Response::ACK,  Response::NACK },
    { 0x1E << 1, Response::NACK, Response::ACK  },
    { 0x1F << 1, Response::ACK,  Response::NACK },

    { 0x20 << 1, Response::NACK, Response::ACK  },
    { 0x21 << 1, Response::NACK, Response::ACK  },
    { 0x22 << 1, Response::ACK,  Response::NACK },
    { 0x23 << 1, Response::NACK, Response::ACK  },
    { 0x24 << 1, Response::NACK, Response::NACK },
    { 0x25 << 1, Response::NACK, Response::ACK  },
    { 0x26 << 1, Response::ACK,  Response::ACK  },
    { 0x27 << 1, Response::NACK, Response::ACK  },
    { 0x28 << 1, Response::ACK,  Response::ACK  },
    { 0x29 << 1, Response::NACK, Response::ACK  },
    { 0x2A << 1, Response::NACK, Response::NACK },
    { 0x2B << 1, Response::NACK, Response::ACK  },
    { 0x2C << 1, Response::NACK, Response::NACK },
    { 0x2D << 1, Response::ACK,  Response::ACK  },
    { 0x2E << 1, Response::ACK,  Response::NACK },
    { 0x2F << 1, Response::NACK, Response::NACK },

    { 0x30 << 1, Response::ACK,  Response::NACK },
    { 0x31 << 1, Response::ACK,  Response::ACK  },
    { 0x32 << 1, Response::NACK, Response::ACK  },
    { 0x33 << 1, Response::ACK,  Response::NACK },
    { 0x34 << 1, Response::NACK, Response::NACK },
    { 0x35 << 1, Response::ACK,  Response::NACK },
    { 0x36 << 1, Response::NACK, Response::NACK },
    { 0x37 << 1, Response::ACK,  Response::NACK },
    { 0x38 << 1, Response::ACK,  Response::NACK },
    { 0x39 << 1, Response::ACK,  Response::ACK  },
    { 0x3A << 1, Response::NACK, Response::ACK  },
    { 0x3B << 1, Response::NACK, Response::ACK  },
    { 0x3C << 1, Response::ACK,  Response::NACK },
    { 0x3D << 1, Response::ACK,  Response::ACK  },
    { 0x3E << 1, Response::NACK, Response::ACK  },
    { 0x3F << 1, Response::ACK,  Response::ACK  },

    { 0x40 << 1, Response::ACK,  Response::NACK },
    { 0x41 << 1, Response::ACK,  Response::ACK  },
    { 0x42 << 1, Response::NACK, Response::NACK },
    { 0x43 << 1, Response::ACK,  Response::ACK  },
    { 0x44 << 1, Response::ACK,  Response::NACK },
    { 0x45 << 1, Response::ACK,  Response::NACK },
    { 0x46 << 1, Response::ACK,  Response::ACK  },
    { 0x47 << 1, Response::ACK,  Response::NACK },
    { 0x48 << 1, Response::NACK, Response::ACK  },
    { 0x49 << 1, Response::ACK,  Response::ACK  },
    { 0x4A << 1, Response::ACK,  Response::ACK  },
    { 0x4B << 1, Response::NACK, Response::NACK },
    { 0x4C << 1, Response::NACK, Response::NACK },
    { 0x4D << 1, Response::ACK,  Response::NACK },
    { 0x4E << 1, Response::NACK, Response::ACK  },
    { 0x4F << 1, Response::NACK, Response::NACK },

    { 0x50 << 1, Response::ACK,  Response::ACK  },
    { 0x51 << 1, Response::NACK, Response::ACK  },
    { 0x52 << 1, Response::ACK,  Response::ACK  },
    { 0x53 << 1, Response::ACK,  Response::ACK  },
    { 0x54 << 1, Response::ACK,  Response::NACK },
    { 0x55 << 1, Response::ACK,  Response::ACK  },
    { 0x56 << 1, Response::NACK, Response::ACK  },
    { 0x57 << 1, Response::NACK, Response::NACK },
    { 0x58 << 1, Response::ACK,  Response::ACK  },
    { 0x59 << 1, Response::ACK,  Response::ACK  },
    { 0x5A << 1, Response::ACK,  Response::NACK },
    { 0x5B << 1, Response::ACK,  Response::NACK },
    { 0x5C << 1, Response::NACK, Response::NACK },
    { 0x5D << 1, Response::ACK,  Response::ACK  },
    { 0x5E << 1, Response::NACK, Response::NACK },
    { 0x5F << 1, Response::ACK,  Response::NACK },

    { 0x60 << 1, Response::ACK,  Response::ACK  },
    { 0x61 << 1, Response::NACK, Response::ACK  },
    { 0x62 << 1, Response::NACK, Response::ACK  },
    { 0x63 << 1, Response::NACK, Response::NACK },
    { 0x64 << 1, Response::ACK,  Response::ACK  },
    { 0x65 << 1, Response::ACK,  Response::NACK },
    { 0x66 << 1, Response::ACK,  Response::ACK  },
    { 0x67 << 1, Response::NACK, Response::NACK },
    { 0x68 << 1, Response::NACK, Response::NACK },
    { 0x69 << 1, Response::ACK,  Response::NACK },
    { 0x6A << 1, Response::ACK,  Response::NACK },
    { 0x6B << 1, Response::ACK,  Response::NACK },
    { 0x6C << 1, Response::ACK,  Response::ACK  },
    { 0x6D << 1, Response::NACK, Response::ACK  },
    { 0x6E << 1, Response::NACK, Response::NACK },
    { 0x6F << 1, Response::NACK, Response::ACK  },

    { 0x70 << 1, Response::NACK, Response::ACK  },
    { 0x71 << 1, Response::NACK, Response::NACK },
    { 0x72 << 1, Response::ACK,  Response::ACK  },
    { 0x73 << 1, Response::NACK, Response::NACK },
    { 0x74 << 1, Response::NACK, Response::NACK },
    { 0x75 << 1, Response::ACK,  Response::ACK  },
    { 0x76 << 1, Response::ACK,  Response::NACK },
    { 0x77 << 1, Response::NACK, Response::NACK },
    { 0x78 << 1, Response::ACK,  Response::ACK  },
    { 0x79 << 1, Response::NACK, Response::ACK  },
    { 0x7A << 1, Response::ACK,  Response::NACK },
    { 0x7B << 1, Response::ACK,  Response::ACK  },
    { 0x7C << 1, Response::ACK,  Response::ACK  },
    { 0x7D << 1, Response::NACK, Response::NACK },
    { 0x7E << 1, Response::NACK, Response::ACK  },
    { 0x7F << 1, Response::ACK,  Response::NACK },

    // clang-format on
};

/**
 * \brief Verify picolibrary::I2C::scan( Controller &, Functor ) works properly.
 */
TEST( scanControllerFunctor, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};
    auto functor    = MockFunction<void( Address_Transmitted, Operation, Response )>{};

    for ( auto const device : scan_DEVICES ) {
        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::READ ) )
            .WillOnce( Return( device.response_read ) );
        if ( device.response_read == Response::ACK ) {
            EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xE0 ) );
        } // if
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::READ, device.response_read ) );

        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::WRITE ) )
            .WillOnce( Return( device.response_write ) );
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::WRITE, device.response_write ) );
    } // for

    scan( controller, functor.AsStdFunction() );
}

/**
 * \brief Verify picolibrary::I2C::scan( Controller &, Functor,
 *        picolibrary::Functor_Can_Fail_Return_Functor ) properly handles a functor error.
 */
TEST( scanControllerFunctorFunctorCanFailReturnFunctorErrorHandling, functorError )
{
    auto controller = Mock_Controller{};
    auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

    auto const error = Mock_Error{ 0xDA };

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Response::NACK ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = scan<Functor_Can_Fail_Return_Functor>( controller, functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan( Controller &, Functor,
 *        picolibrary::Functor_Can_Fail_Return_Functor ) works properly.
 */
TEST( scanControllerFunctorFunctorCanFailReturnFunctor, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};
    auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

    for ( auto const device : scan_DEVICES ) {
        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::READ ) )
            .WillOnce( Return( device.response_read ) );
        if ( device.response_read == Response::ACK ) {
            EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xE0 ) );
        } // if
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::READ, device.response_read ) )
            .WillOnce( Return( Result<void>{} ) );

        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::WRITE ) )
            .WillOnce( Return( device.response_write ) );
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::WRITE, device.response_write ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    auto const result = scan<Functor_Can_Fail_Return_Functor>( controller, functor.AsStdFunction() );

    static_assert( std::is_same_v<decltype( result )::Value, decltype( functor.AsStdFunction() )> );

    ASSERT_FALSE( result.is_error() );

    EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( Result<void>{} ) );

    ASSERT_FALSE( result.value()( 0x18 << 1, Operation::WRITE, Response::NACK ).is_error() );
}

/**
 * \brief Verify picolibrary::I2C::scan( Controller &, Functor,
 *        picolibrary::Functor_Can_Fail_Discard_Functor ) properly handles a functor
 *        error.
 */
TEST( scanControllerFunctorFunctorCanFailDiscardFunctorErrorHandling, functorError )
{
    auto controller = Mock_Controller{};
    auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

    auto const error = Mock_Error{ 0xDA };

    EXPECT_CALL( controller, start() );
    EXPECT_CALL( controller, address( _, _ ) ).WillOnce( Return( Response::NACK ) );
    EXPECT_CALL( controller, stop() );

    EXPECT_CALL( functor, Call( _, _, _ ) ).WillOnce( Return( error ) );

    auto const result = scan<Functor_Can_Fail_Discard_Functor>( controller, functor.AsStdFunction() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::I2C::scan( Controller &, Functor,
 *        picolibrary::Functor_Can_Fail_Discard_Functor ) works properly.
 */
TEST( scanControllerFunctorFunctorCanFailDiscardFunctor, worksProperly )
{
    auto const in_sequence = InSequence{};

    auto controller = Mock_Controller{};
    auto functor = MockFunction<Result<void>( Address_Transmitted, Operation, Response )>{};

    for ( auto const device : scan_DEVICES ) {
        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::READ ) )
            .WillOnce( Return( device.response_read ) );
        if ( device.response_read == Response::ACK ) {
            EXPECT_CALL( controller, read( Response::NACK ) ).WillOnce( Return( 0xE0 ) );
        } // if
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::READ, device.response_read ) )
            .WillOnce( Return( Result<void>{} ) );

        EXPECT_CALL( controller, start() );
        EXPECT_CALL( controller, address( device.address, Operation::WRITE ) )
            .WillOnce( Return( device.response_write ) );
        EXPECT_CALL( controller, stop() );

        EXPECT_CALL( functor, Call( device.address, Operation::WRITE, device.response_write ) )
            .WillOnce( Return( Result<void>{} ) );
    } // for

    auto const result = scan<Functor_Can_Fail_Discard_Functor>( controller, functor.AsStdFunction() );

    static_assert( std::is_same_v<decltype( result )::Value, void> );

    ASSERT_FALSE( result.is_error() );
}

/**
 * \brief Execute the picolibrary::I2C automated tests.
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
