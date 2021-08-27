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
 * \brief picolibrary::IPv4::Address unit test program.
 */

#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Return;

auto random_unique_unsigned_integers()
{
    auto const a = random<Address::Unsigned_Integer>();
    auto const b = random<Address::Unsigned_Integer>();

    return std::pair<Address::Unsigned_Integer, Address::Unsigned_Integer>{
        a, b != a ? b : b ^ random<Address::Unsigned_Integer>( 1 )
    };
}

constexpr auto convert_byte_array_to_unsigned_integer( Address::Byte_Array const & byte_array ) noexcept
{
    return static_cast<Address::Unsigned_Integer>(
        ( static_cast<Address::Unsigned_Integer>( byte_array[ 0 ] )
          << 3 * std::numeric_limits<std::uint8_t>::digits )
        | ( static_cast<Address::Unsigned_Integer>( byte_array[ 1 ] )
            << 2 * std::numeric_limits<std::uint8_t>::digits )
        | ( static_cast<Address::Unsigned_Integer>( byte_array[ 2 ] )
            << 1 * std::numeric_limits<std::uint8_t>::digits )
        | ( static_cast<Address::Unsigned_Integer>( byte_array[ 3 ] )
            << 0 * std::numeric_limits<std::uint8_t>::digits ) );
}

constexpr auto convert_unsigned_integer_to_byte_array( Address::Unsigned_Integer unsigned_integer ) noexcept
{
    return Address::Byte_Array{
        static_cast<std::uint8_t>( unsigned_integer >> 3 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 2 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 1 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 0 * std::numeric_limits<std::uint8_t>::digits ),
    };
}

constexpr auto ANY_AS_BYTE_ARRAY = Address::Byte_Array{ 0, 0, 0, 0 };

constexpr auto ANY_AS_UNSIGNED_INTEGER = convert_byte_array_to_unsigned_integer( ANY_AS_BYTE_ARRAY );

constexpr auto LOOPBACK_AS_BYTE_ARRAY = Address::Byte_Array{ 127, 0, 0, 1 };

constexpr auto LOOPBACK_AS_UNSIGNED_INTEGER = convert_byte_array_to_unsigned_integer( LOOPBACK_AS_BYTE_ARRAY );

auto dot_decimal( Address::Byte_Array const & byte_array )
{
    auto stream = std::ostringstream{};

    stream << static_cast<std::uint_fast16_t>( byte_array[ 0 ] ) << '.'
           << static_cast<std::uint_fast16_t>( byte_array[ 1 ] ) << '.'
           << static_cast<std::uint_fast16_t>( byte_array[ 2 ] ) << '.'
           << static_cast<std::uint_fast16_t>( byte_array[ 3 ] );

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::IPv4::Address::any() works properly.
 */
TEST( any, worksProperly )
{
    auto const address = Address::any();

    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_EQ( address.as_byte_array(), ANY_AS_BYTE_ARRAY );
    EXPECT_EQ( address.as_unsigned_integer(), ANY_AS_UNSIGNED_INTEGER );
}

/**
 * \brief Verify picolibrary::IPv4::Address::loopback() works properly.
 */
TEST( loopback, worksProperly )
{
    auto const address = Address::loopback();

    EXPECT_FALSE( address.is_any() );
    EXPECT_TRUE( address.is_loopback() );
    EXPECT_EQ( address.as_byte_array(), LOOPBACK_AS_BYTE_ARRAY );
    EXPECT_EQ( address.as_unsigned_integer(), LOOPBACK_AS_UNSIGNED_INTEGER );
}

/**
 * \brief Verify picolibrary::IPv4::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_EQ( address.as_byte_array(), ANY_AS_BYTE_ARRAY );
    EXPECT_EQ( address.as_unsigned_integer(), ANY_AS_UNSIGNED_INTEGER );
}

/**
 * \brief Verify picolibrary::IPv4::Address( picolibrary::IPv4::Address::Byte_Array const
 *        & ) works properly.
 */
TEST( constructorByteArray, worksProperly )
{
    auto const unsigned_integer = random<Address::Unsigned_Integer>();
    auto const byte_array = convert_unsigned_integer_to_byte_array( unsigned_integer );

    auto const address = Address{ byte_array };

    EXPECT_EQ( address.is_any(), byte_array == ANY_AS_BYTE_ARRAY );
    EXPECT_EQ( address.is_loopback(), byte_array == LOOPBACK_AS_BYTE_ARRAY );
    EXPECT_EQ( address.as_byte_array(), byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief Verify picolibrary::IPv4::Address( picolibrary::IPv4::Address::Unsigned_Integer
 *        ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    auto const unsigned_integer = random<Address::Unsigned_Integer>();
    auto const byte_array = convert_unsigned_integer_to_byte_array( unsigned_integer );

    auto const address = Address{ unsigned_integer };

    EXPECT_EQ( address.is_any(), byte_array == ANY_AS_BYTE_ARRAY );
    EXPECT_EQ( address.is_loopback(), byte_array == LOOPBACK_AS_BYTE_ARRAY );
    EXPECT_EQ( address.as_byte_array(), byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief Verify picolibrary::IPv4::operator==( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random<Address::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_TRUE( Address{ lhs } == Address{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_FALSE( Address{ lhs } == Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IPv4::operator!=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random<Address::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_FALSE( Address{ lhs } != Address{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_TRUE( Address{ lhs } != Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IPv4::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random<Address::Unsigned_Integer>( 1 );
        auto const lhs = random<Address::Unsigned_Integer>( 0, rhs - 1 );

        EXPECT_TRUE( Address{ lhs } < Address{ rhs } );
    }

    {
        auto const rhs = random<Address::Unsigned_Integer>();
        auto const lhs = random<Address::Unsigned_Integer>( rhs );

        EXPECT_FALSE( Address{ lhs } < Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IPv4::operator>( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random<Address::Unsigned_Integer>( 1 );
        auto const rhs = random<Address::Unsigned_Integer>( 0, lhs - 1 );

        EXPECT_TRUE( Address{ lhs } > Address{ rhs } );
    }

    {
        auto const lhs = random<Address::Unsigned_Integer>();
        auto const rhs = random<Address::Unsigned_Integer>( lhs );

        EXPECT_FALSE( Address{ lhs } > Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IPv4::operator<=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random<Address::Unsigned_Integer>();
        auto const rhs = random<Address::Unsigned_Integer>( lhs );

        EXPECT_TRUE( Address{ lhs } <= Address{ rhs } );
    }

    {
        auto const lhs = random<Address::Unsigned_Integer>( 1 );
        auto const rhs = random<Address::Unsigned_Integer>( 0, lhs - 1 );

        EXPECT_FALSE( Address{ lhs } <= Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IPv4::operator>=( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random<Address::Unsigned_Integer>();
        auto const lhs = random<Address::Unsigned_Integer>( rhs );

        EXPECT_TRUE( Address{ lhs } >= Address{ rhs } );
    }

    {
        auto const rhs = random<Address::Unsigned_Integer>( 1 );
        auto const lhs = random<Address::Unsigned_Integer>( 0, rhs - 1 );

        EXPECT_FALSE( Address{ lhs } >= Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address> properly
 *        handles an invalid format string.
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
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address> properly
 *        handles a print error.
 */
TEST( outputFormatter, printError )
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

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address> works properly.
 */
TEST( outputFormatter, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const address = Address{ random<Address::Unsigned_Integer>() };

    EXPECT_FALSE( stream.print( "{}", address ).is_error() );

    EXPECT_EQ( stream.string(), dot_decimal( address.as_byte_array() ) );
}

/**
 * \brief Execute the picolibrary::IPv4::Address unit tests.
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
