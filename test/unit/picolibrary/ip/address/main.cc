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
 * \brief picolibrary::IP::Address unit test program.
 */

#include <cstdint>
#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/ipv4.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::IP::Address;
using ::picolibrary::IP::Version;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Return;

using IPv4_Address = ::picolibrary::IPv4::Address;

auto dot_decimal( IPv4_Address const & address )
{
    auto stream = std::ostringstream{};

    stream << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 0 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 1 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 2 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 3 ] );

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::IP::Address::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_EQ( address.version(), Version::UNSPECIFIED );
    EXPECT_TRUE( address.is_unspecified() );
    EXPECT_FALSE( address.is_ipv4() );
    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_FALSE( address.is_multicast() );
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IPv4::Address const & )
 *        works properly.
 */
TEST( constructorIPv4Address, worksProperly )
{
    {
        auto const ipv4_address = IPv4_Address::any();

        auto const address = Address{ ipv4_address };

        EXPECT_EQ( address.version(), Version::_4 );
        EXPECT_FALSE( address.is_unspecified() );
        EXPECT_TRUE( address.is_ipv4() );
        EXPECT_TRUE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.ipv4(), ipv4_address );
    }

    {
        auto const ipv4_address = IPv4_Address::loopback();

        auto const address = Address{ ipv4_address };

        EXPECT_EQ( address.version(), Version::_4 );
        EXPECT_FALSE( address.is_unspecified() );
        EXPECT_TRUE( address.is_ipv4() );
        EXPECT_FALSE( address.is_any() );
        EXPECT_TRUE( address.is_loopback() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.ipv4(), ipv4_address );
    }

    {
        auto const ipv4_address = random<IPv4_Address>();

        auto const address = Address{ ipv4_address };

        EXPECT_EQ( address.version(), Version::_4 );
        EXPECT_FALSE( address.is_unspecified() );
        EXPECT_TRUE( address.is_ipv4() );
        EXPECT_EQ( address.is_any(), ipv4_address.is_any() );
        EXPECT_EQ( address.is_loopback(), ipv4_address.is_loopback() );
        EXPECT_EQ( address.is_multicast(), ipv4_address.is_multicast() );
        EXPECT_EQ( address.ipv4(), ipv4_address );
    }
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address && ) works
 *        properly.
 */
TEST( constructorMove, worksProperly )
{
    {
        auto source = Address{};

        auto const address = Address{ std::move( source ) };

        EXPECT_EQ( address.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( address.is_unspecified() );
        EXPECT_FALSE( address.is_ipv4() );
        EXPECT_TRUE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_multicast() );

        EXPECT_EQ( source.version(), address.version() );
        EXPECT_EQ( source.is_unspecified(), address.is_unspecified() );
        EXPECT_EQ( source.is_ipv4(), address.is_ipv4() );
        EXPECT_EQ( source.is_any(), address.is_any() );
        EXPECT_EQ( source.is_loopback(), address.is_loopback() );
        EXPECT_EQ( source.is_multicast(), address.is_multicast() );
    }

    {
        auto const source_ipv4_address = random<IPv4_Address>();

        auto source = Address{ source_ipv4_address };

        auto const address = Address{ std::move( source ) };

        EXPECT_EQ( address.version(), Version::_4 );
        EXPECT_FALSE( address.is_unspecified() );
        EXPECT_TRUE( address.is_ipv4() );
        EXPECT_EQ( address.is_any(), source_ipv4_address.is_any() );
        EXPECT_EQ( address.is_loopback(), source_ipv4_address.is_loopback() );
        EXPECT_EQ( address.is_multicast(), source_ipv4_address.is_multicast() );
        EXPECT_EQ( address.ipv4(), source_ipv4_address );

        EXPECT_EQ( source.version(), address.version() );
        EXPECT_EQ( source.is_unspecified(), address.is_unspecified() );
        EXPECT_EQ( source.is_ipv4(), address.is_ipv4() );
        EXPECT_EQ( source.is_any(), address.is_any() );
        EXPECT_EQ( source.is_loopback(), address.is_loopback() );
        EXPECT_EQ( source.is_multicast(), address.is_multicast() );
        EXPECT_EQ( source.ipv4(), address.ipv4() );
    }
}

/**
 * \brief Verify picolibrary::IP::Address::Address( picolibrary::IP::Address const & )
 *        works properly.
 */
TEST( constructorCopy, worksProperly )
{
    {
        auto const original = Address{};

        auto const address = Address{ original };

        EXPECT_EQ( address.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( address.is_unspecified() );
        EXPECT_FALSE( address.is_ipv4() );
        EXPECT_TRUE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_multicast() );
    }

    {
        auto const original_ipv4_address = random<IPv4_Address>();

        auto const original = Address{ original_ipv4_address };

        auto const address = Address{ original };

        EXPECT_EQ( address.version(), Version::_4 );
        EXPECT_FALSE( address.is_unspecified() );
        EXPECT_TRUE( address.is_ipv4() );
        EXPECT_EQ( address.is_any(), original_ipv4_address.is_any() );
        EXPECT_EQ( address.is_loopback(), original_ipv4_address.is_loopback() );
        EXPECT_EQ( address.is_multicast(), original_ipv4_address.is_multicast() );
        EXPECT_EQ( address.ipv4(), original_ipv4_address );
    }
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address && ) works
 *        properly.
 */
TEST( assignmentOperatorMove, worksProperly )
{
    {
        auto expression = Address{};
        auto object     = Address{};

        object = std::move( expression );

        EXPECT_EQ( object.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( object.is_unspecified() );
        EXPECT_FALSE( object.is_ipv4() );
        EXPECT_TRUE( object.is_any() );
        EXPECT_FALSE( object.is_loopback() );
        EXPECT_FALSE( object.is_multicast() );

        EXPECT_EQ( expression.version(), object.version() );
        EXPECT_EQ( expression.is_unspecified(), object.is_unspecified() );
        EXPECT_EQ( expression.is_ipv4(), object.is_ipv4() );
        EXPECT_EQ( expression.is_any(), object.is_any() );
        EXPECT_EQ( expression.is_loopback(), object.is_loopback() );
        EXPECT_EQ( expression.is_multicast(), object.is_multicast() );
    }

    {
        auto const expression_ipv4_address = random<IPv4_Address>();

        auto expression = Address{ expression_ipv4_address };
        auto object     = Address{ random<IPv4_Address>() };

        object = std::move( expression );

        EXPECT_EQ( object.version(), Version::_4 );
        EXPECT_FALSE( object.is_unspecified() );
        EXPECT_TRUE( object.is_ipv4() );
        EXPECT_EQ( object.is_any(), expression_ipv4_address.is_any() );
        EXPECT_EQ( object.is_loopback(), expression_ipv4_address.is_loopback() );
        EXPECT_EQ( object.is_multicast(), expression_ipv4_address.is_multicast() );
        EXPECT_EQ( object.ipv4(), expression_ipv4_address );

        EXPECT_EQ( expression.version(), object.version() );
        EXPECT_EQ( expression.is_unspecified(), object.is_unspecified() );
        EXPECT_EQ( expression.is_ipv4(), object.is_ipv4() );
        EXPECT_EQ( expression.is_any(), object.is_any() );
        EXPECT_EQ( expression.is_loopback(), object.is_loopback() );
        EXPECT_EQ( expression.is_multicast(), object.is_multicast() );
        EXPECT_EQ( expression.ipv4(), object.ipv4() );
    }

    {
        auto const expression_ipv4_address = random<IPv4_Address>();

        auto expression = Address{ expression_ipv4_address };
        auto object     = Address{};

        object = std::move( expression );

        EXPECT_EQ( object.version(), Version::_4 );
        EXPECT_FALSE( object.is_unspecified() );
        EXPECT_TRUE( object.is_ipv4() );
        EXPECT_EQ( object.is_any(), expression_ipv4_address.is_any() );
        EXPECT_EQ( object.is_loopback(), expression_ipv4_address.is_loopback() );
        EXPECT_EQ( object.is_multicast(), expression_ipv4_address.is_multicast() );
        EXPECT_EQ( object.ipv4(), expression_ipv4_address );

        EXPECT_EQ( expression.version(), object.version() );
        EXPECT_EQ( expression.is_unspecified(), object.is_unspecified() );
        EXPECT_EQ( expression.is_ipv4(), object.is_ipv4() );
        EXPECT_EQ( expression.is_any(), object.is_any() );
        EXPECT_EQ( expression.is_loopback(), object.is_loopback() );
        EXPECT_EQ( expression.is_multicast(), object.is_multicast() );
        EXPECT_EQ( expression.ipv4(), object.ipv4() );
    }

    {
        auto expression = Address{};
        auto object     = Address{ random<IPv4_Address>() };

        object = std::move( expression );

        EXPECT_EQ( object.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( object.is_unspecified() );
        EXPECT_FALSE( object.is_ipv4() );
        EXPECT_TRUE( object.is_any() );
        EXPECT_FALSE( object.is_loopback() );
        EXPECT_FALSE( object.is_multicast() );

        EXPECT_EQ( expression.version(), object.version() );
        EXPECT_EQ( expression.is_unspecified(), object.is_unspecified() );
        EXPECT_EQ( expression.is_ipv4(), object.is_ipv4() );
        EXPECT_EQ( expression.is_any(), object.is_any() );
        EXPECT_EQ( expression.is_loopback(), object.is_loopback() );
        EXPECT_EQ( expression.is_multicast(), object.is_multicast() );
    }
}

/**
 * \brief Verify picolibrary::IP::Address::operator=( picolibrary::IP::Address const & )
 *        works properly.
 */
TEST( assignmentOperatorCopy, worksProperly )
{
    {
        auto const expression = Address{};
        auto       object     = Address{};

        object = expression;

        EXPECT_EQ( object.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( object.is_unspecified() );
        EXPECT_FALSE( object.is_ipv4() );
        EXPECT_TRUE( object.is_any() );
        EXPECT_FALSE( object.is_loopback() );
        EXPECT_FALSE( object.is_multicast() );
    }

    {
        auto const expression_ipv4_address = random<IPv4_Address>();

        auto const expression = Address{ expression_ipv4_address };
        auto       object     = Address{ random<IPv4_Address>() };

        object = expression;

        EXPECT_EQ( object.version(), Version::_4 );
        EXPECT_FALSE( object.is_unspecified() );
        EXPECT_TRUE( object.is_ipv4() );
        EXPECT_EQ( object.is_any(), expression_ipv4_address.is_any() );
        EXPECT_EQ( object.is_loopback(), expression_ipv4_address.is_loopback() );
        EXPECT_EQ( object.is_multicast(), expression_ipv4_address.is_multicast() );
        EXPECT_EQ( object.ipv4(), expression_ipv4_address );
    }

    {
        auto const expression_ipv4_address = random<IPv4_Address>();

        auto const expression = Address{ expression_ipv4_address };
        auto       object     = Address{};

        object = expression;

        EXPECT_EQ( object.version(), Version::_4 );
        EXPECT_FALSE( object.is_unspecified() );
        EXPECT_TRUE( object.is_ipv4() );
        EXPECT_EQ( object.is_any(), expression_ipv4_address.is_any() );
        EXPECT_EQ( object.is_loopback(), expression_ipv4_address.is_loopback() );
        EXPECT_EQ( object.is_multicast(), expression_ipv4_address.is_multicast() );
        EXPECT_EQ( object.ipv4(), expression_ipv4_address );
    }

    {
        auto const expression = Address{};
        auto       object     = Address{ random<IPv4_Address>() };

        object = expression;

        EXPECT_EQ( object.version(), Version::UNSPECIFIED );
        EXPECT_TRUE( object.is_unspecified() );
        EXPECT_FALSE( object.is_ipv4() );
        EXPECT_TRUE( object.is_any() );
        EXPECT_FALSE( object.is_loopback() );
        EXPECT_FALSE( object.is_multicast() );
    }
}

/**
 * \brief Verify picolibrary::IP::operator==( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        EXPECT_TRUE( Address{} == Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } == Address{ rhs }, lhs == rhs );
    }

    {
        EXPECT_FALSE( Address{} == Address{ random<IPv4_Address>() } );
    }
}

/**
 * \brief Verify picolibrary::IP::operator!=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        EXPECT_FALSE( Address{} != Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } != Address{ rhs }, lhs != rhs );
    }

    {
        EXPECT_TRUE( Address{} != Address{ random<IPv4_Address>() } );
    }
}

/**
 * \brief Verify picolibrary::IP::operator<( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        EXPECT_FALSE( Address{} < Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } < Address{ rhs }, lhs < rhs );
    }

    {
        EXPECT_TRUE( Address{} < Address{ random<IPv4_Address>() } );
    }

    {
        EXPECT_FALSE( Address{ random<IPv4_Address>() } < Address{} );
    }
}

/**
 * \brief Verify picolibrary::IP::operator>( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        EXPECT_FALSE( Address{} > Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } > Address{ rhs }, lhs > rhs );
    }

    {
        EXPECT_FALSE( Address{} > Address{ random<IPv4_Address>() } );
    }

    {
        EXPECT_TRUE( Address{ random<IPv4_Address>() } > Address{} );
    }
}

/**
 * \brief Verify picolibrary::IP::operator<=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        EXPECT_TRUE( Address{} <= Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } <= Address{ rhs }, lhs <= rhs );
    }

    {
        EXPECT_TRUE( Address{} <= Address{ random<IPv4_Address>() } );
    }

    {
        EXPECT_FALSE( Address{ random<IPv4_Address>() } <= Address{} );
    }
}

/**
 * \brief Verify picolibrary::IP::operator>=( picolibrary::IP::Address const &,
 *        picolibrary::IP::Address const & ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        EXPECT_TRUE( Address{} >= Address{} );
    }

    {
        auto const lhs = random<IPv4_Address>();
        auto const rhs = random<IPv4_Address>();

        EXPECT_EQ( Address{ lhs } >= Address{ rhs }, lhs >= rhs );
    }

    {
        EXPECT_FALSE( Address{} >= Address{ random<IPv4_Address>() } );
    }

    {
        EXPECT_TRUE( Address{ random<IPv4_Address>() } >= Address{} );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address> properly handles
 *        an invalid format string.
 */
TEST( outputFormatter, invalidFormatString )
{
    auto stream = Output_String_Stream{};

    auto const result = stream.print(
        ( std::string{ '{' } + random_container<std::string>( random<std::uint_fast8_t>( 1 ) ) + '}' )
            .c_str(),
        Address{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_FORMAT );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_TRUE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address> properly handles
 *        a print error.
 */
TEST( outputFormatter, printError )
{
    {
        auto stream = Mock_Output_Stream{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

        auto const result = stream.print( "{}", Address{} );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_FALSE( stream.end_of_file_reached() );
        EXPECT_FALSE( stream.io_error_present() );
        EXPECT_TRUE( stream.fatal_error_present() );
    }

    {
        auto stream = Mock_Output_Stream{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

        auto const result = stream.print(
            "{}", Address{ random<IPv4_Address>( { { 0, 0, 0, 1 } } ) } );

        EXPECT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_FALSE( stream.end_of_file_reached() );
        EXPECT_FALSE( stream.io_error_present() );
        EXPECT_TRUE( stream.fatal_error_present() );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Address> works properly.
 */
TEST( outputFormatter, worksProperly )
{
    {
        auto stream = Output_String_Stream{};

        EXPECT_FALSE( stream.print( "{}", Address{} ).is_error() );

        EXPECT_EQ( stream.string(), "ANY" );
    }

    {
        auto stream = Output_String_Stream{};

        auto const ipv4_address = random<IPv4_Address>( { { 0, 0, 0, 1 } } );

        EXPECT_FALSE( stream.print( "{}", Address{ ipv4_address } ).is_error() );

        EXPECT_EQ( stream.string(), dot_decimal( ipv4_address ) );
    }
}

/**
 * \brief Execute the picolibrary::IP::Address unit tests.
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