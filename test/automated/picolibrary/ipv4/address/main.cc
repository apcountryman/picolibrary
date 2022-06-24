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
 * \brief picolibrary::IPv4::Address automated test program.
 */

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IPv4::Address;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_array;
using ::picolibrary::Testing::Automated::random_unique_pair;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;

auto to_unsigned_integer( Address::Byte_Array byte_array ) -> Address::Unsigned_Integer
{
    // clang-format off

    return static_cast<Address::Unsigned_Integer>(
        ( static_cast<Address::Unsigned_Integer>( byte_array[ 0 ] ) << 3 *std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<Address::Unsigned_Integer>( byte_array[ 1 ] ) << 2 *std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<Address::Unsigned_Integer>( byte_array[ 2 ] ) << 1 *std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<Address::Unsigned_Integer>( byte_array[ 3 ] ) << 0 *std::numeric_limits<std::uint8_t>::digits ) );

    // clang-format on
}

auto dot_decimal( Address::Byte_Array const & byte_array ) -> std::string
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
 * \brief Verify picolibrary::IPv4::Address::Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = Address{};

    EXPECT_TRUE( address.is_any() );
    EXPECT_FALSE( address.is_loopback() );
    EXPECT_FALSE( address.is_broadcast() );
    EXPECT_FALSE( address.is_multicast() );
    EXPECT_EQ( address.as_byte_array(), ( Address::Byte_Array{ 0, 0, 0, 0 } ) );
    EXPECT_EQ( address.as_unsigned_integer(), 0'0'0'0 );
}

/**
 * \brief Verify picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Byte_Array const & ) works properly.
 */
TEST( constructorByteArray, worksProperly )
{
    {
        auto const byte_array = Address::Byte_Array{ 0, 0, 0, 0 };

        auto const address = Address{ byte_array };

        EXPECT_TRUE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    }

    {
        auto const byte_array = Address::Byte_Array{
            127, random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>()
        };

        auto const address = Address{ byte_array };

        EXPECT_FALSE( address.is_any() );
        EXPECT_TRUE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    }

    {
        auto const byte_array = Address::Byte_Array{ 255, 255, 255, 255 };

        auto const address = Address{ byte_array };

        EXPECT_FALSE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_TRUE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    }

    {
        auto const byte_array = Address::Byte_Array{ random<std::uint8_t>( 224, 239 ),
                                                     random<std::uint8_t>(),
                                                     random<std::uint8_t>(),
                                                     random<std::uint8_t>() };

        auto const address = Address{ byte_array };

        EXPECT_FALSE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_TRUE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    }

    {
        auto const byte_array = random_array<std::uint8_t, 4>();

        auto const address = Address{ byte_array };

        EXPECT_EQ( address.is_any(), ( byte_array == Address::Byte_Array{ 0, 0, 0, 0 } ) );
        EXPECT_EQ( address.is_loopback(), byte_array[ 0 ] == 127 );
        EXPECT_EQ( address.is_broadcast(), ( byte_array == Address::Byte_Array{ 255, 255, 255, 255 } ) );
        EXPECT_EQ( address.is_multicast(), byte_array[ 0 ] >= 224 and byte_array[ 0 ] <= 239 );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    }
}

/**
 * \brief Verify picolibrary::IPv4::Address::Address(
 *        picolibrary::IPv4::Address::Unsigned_Integer const & ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    {
        auto const byte_array       = Address::Byte_Array{ 0, 0, 0, 0 };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = Address{ unsigned_integer };

        EXPECT_TRUE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    }

    {
        auto const byte_array = Address::Byte_Array{
            127, random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>()
        };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = Address{ unsigned_integer };

        EXPECT_FALSE( address.is_any() );
        EXPECT_TRUE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    }

    {
        auto const byte_array       = Address::Byte_Array{ 255, 255, 255, 255 };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = Address{ unsigned_integer };

        EXPECT_FALSE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_TRUE( address.is_broadcast() );
        EXPECT_FALSE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    }

    {
        auto const byte_array = Address::Byte_Array{ random<std::uint8_t>( 224, 239 ),
                                                     random<std::uint8_t>(),
                                                     random<std::uint8_t>(),
                                                     random<std::uint8_t>() };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = Address{ unsigned_integer };

        EXPECT_FALSE( address.is_any() );
        EXPECT_FALSE( address.is_loopback() );
        EXPECT_FALSE( address.is_broadcast() );
        EXPECT_TRUE( address.is_multicast() );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    }

    {
        auto const byte_array       = random_array<std::uint8_t, 4>();
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = Address{ unsigned_integer };

        EXPECT_EQ( address.is_any(), ( byte_array == Address::Byte_Array{ 0, 0, 0, 0 } ) );
        EXPECT_EQ( address.is_loopback(), byte_array[ 0 ] == 127 );
        EXPECT_EQ( address.is_broadcast(), ( byte_array == Address::Byte_Array{ 255, 255, 255, 255 } ) );
        EXPECT_EQ( address.is_multicast(), byte_array[ 0 ] >= 224 and byte_array[ 0 ] <= 239 );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    }
}

/**
 * \brief Verify picolibrary::operator==( picolibrary::IPv4::Address const &,
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
        auto const [ lhs, rhs ] = random_unique_pair<Address::Unsigned_Integer>();

        EXPECT_FALSE( Address{ lhs } == Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator!=( picolibrary::IPv4::Address const &,
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
        auto const [ lhs, rhs ] = random_unique_pair<Address::Unsigned_Integer>();

        EXPECT_TRUE( Address{ lhs } != Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator<( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random<Address::Unsigned_Integer>( 0'0'0'0 + 1 );
        auto const lhs = random<Address::Unsigned_Integer>( 0'0'0'0, rhs - 1 );

        EXPECT_TRUE( Address{ lhs } < Address{ rhs } );
    }

    {
        auto const rhs = random<Address::Unsigned_Integer>();
        auto const lhs = random<Address::Unsigned_Integer>( rhs );

        EXPECT_FALSE( Address{ lhs } < Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator>( picolibrary::IPv4::Address const &,
 *        picolibrary::IPv4::Address const & ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random<Address::Unsigned_Integer>( 0'0'0'0 + 1 );
        auto const rhs = random<Address::Unsigned_Integer>( 0'0'0'0, lhs - 1 );

        EXPECT_TRUE( Address{ lhs } > Address{ rhs } );
    }

    {
        auto const lhs = random<Address::Unsigned_Integer>();
        auto const rhs = random<Address::Unsigned_Integer>( lhs );

        EXPECT_FALSE( Address{ lhs } > Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator<=( picolibrary::IPv4::Address const &,
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
        auto const lhs = random<Address::Unsigned_Integer>( 0'0'0'0 + 1 );
        auto const rhs = random<Address::Unsigned_Integer>( 0'0'0'0, lhs - 1 );

        EXPECT_FALSE( Address{ lhs } <= Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator>=( picolibrary::IPv4::Address const &,
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
        auto const rhs = random<Address::Unsigned_Integer>( 0'0'0'0 + 1 );
        auto const lhs = random<Address::Unsigned_Integer>( 0'0'0'0, rhs - 1 );

        EXPECT_FALSE( Address{ lhs } >= Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address> properly
 *        handles a print error.
 */
TEST( outputFormatterIPv4Address, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Address{ random<Address::Unsigned_Integer>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IPv4::Address> works properly.
 */
TEST( outputFormatterIPv4Address, worksProperly )
{
    {
        auto stream = Output_String_Stream{};

        auto const byte_array = random_array<std::uint8_t, 4>();

        auto const address = Address{ byte_array };

        auto const result = stream.print( address );

        ASSERT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dot_decimal( byte_array ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const byte_array = random_array<std::uint8_t, 4>();

        auto const address = Address{ byte_array };

        auto const n = stream.print( address );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dot_decimal( byte_array ) );
    }
}

/**
 * \brief Execute the picolibrary::IPv4::Address automated tests.
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
