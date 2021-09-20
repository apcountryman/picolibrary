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
 * \brief picolibrary::MAC_Address unit test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <limits>
#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::MAC_Address;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Return;

auto random_unsigned_integer(
    MAC_Address::Unsigned_Integer min = MAC_Address::min().as_unsigned_integer(),
    MAC_Address::Unsigned_Integer max = MAC_Address::max().as_unsigned_integer() )
{
    return random<MAC_Address::Unsigned_Integer>( min, max );
}

auto random_unique_unsigned_integers()
{
    auto const a = random_unsigned_integer();
    auto const b = random_unsigned_integer();

    return std::pair<MAC_Address::Unsigned_Integer, MAC_Address::Unsigned_Integer>{
        a, b != a ? b : b ^ random_unsigned_integer( 1 )
    };
}

constexpr auto convert_unsigned_integer_to_byte_array( MAC_Address::Unsigned_Integer unsigned_integer ) noexcept
{
    return MAC_Address::Byte_Array{
        static_cast<std::uint8_t>( unsigned_integer >> 5 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 4 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 3 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 2 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 1 * std::numeric_limits<std::uint8_t>::digits ),
        static_cast<std::uint8_t>( unsigned_integer >> 0 * std::numeric_limits<std::uint8_t>::digits ),
    };
}

auto hyphen_separated_hexadecimal_digit_pairs( MAC_Address::Byte_Array const & byte_array )
{
    auto stream = std::ostringstream{};

    stream << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 0 ] ) << '-' << std::setw( 2 )
           << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 1 ] ) << '-' << std::setw( 2 )
           << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 2 ] ) << '-' << std::setw( 2 )
           << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 3 ] ) << '-' << std::setw( 2 )
           << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 4 ] ) << '-' << std::setw( 2 )
           << std::setfill( '0' ) << std::hex << std::uppercase
           << static_cast<std::uint_fast16_t>( byte_array[ 5 ] );

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const address = MAC_Address{};

    EXPECT_FALSE( address.is_locally_administered() );
    EXPECT_TRUE( address.is_universally_administered() );
    EXPECT_FALSE( address.is_multicast() );
    EXPECT_TRUE( address.is_unicast() );
    EXPECT_EQ( address.as_byte_array(), ( MAC_Address::Byte_Array{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } ) );
    EXPECT_EQ( address.as_unsigned_integer(), 0 );
}

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Byte_Array const & ) works properly.
 */
TEST( constructorByteArray, worksProperly )
{
    auto const unsigned_integer = random_unsigned_integer();
    auto const byte_array = convert_unsigned_integer_to_byte_array( unsigned_integer );

    auto const address = MAC_Address{ byte_array };

    EXPECT_EQ( address.is_locally_administered(), static_cast<bool>( byte_array[ 0 ] & 0b0000'0010 ) );
    EXPECT_EQ( address.is_universally_administered(), not static_cast<bool>( byte_array[ 0 ] & 0b0000'0010 ) );
    EXPECT_EQ( address.is_multicast(), static_cast<bool>( byte_array[ 0 ] & 0b0000'0001 ) );
    EXPECT_EQ( address.is_unicast(), not static_cast<bool>( byte_array[ 0 ] & 0b0000'0001 ) );
    EXPECT_EQ( address.as_byte_array(), byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    auto const unsigned_integer = random_unsigned_integer();
    auto const byte_array = convert_unsigned_integer_to_byte_array( unsigned_integer );

    auto const address = MAC_Address{ unsigned_integer };

    EXPECT_EQ( address.is_locally_administered(), static_cast<bool>( byte_array[ 0 ] & 0b0000'0010 ) );
    EXPECT_EQ( address.is_universally_administered(), not static_cast<bool>( byte_array[ 0 ] & 0b0000'0010 ) );
    EXPECT_EQ( address.is_multicast(), static_cast<bool>( byte_array[ 0 ] & 0b0000'0001 ) );
    EXPECT_EQ( address.is_unicast(), not static_cast<bool>( byte_array[ 0 ] & 0b0000'0001 ) );
    EXPECT_EQ( address.as_byte_array(), byte_array );
    EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief Verify picolibrary::operator==( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random_unsigned_integer();
        auto const rhs = lhs;

        EXPECT_TRUE( MAC_Address{ lhs } == MAC_Address{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_FALSE( MAC_Address{ lhs } == MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator!=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random_unsigned_integer();
        auto const rhs = lhs;

        EXPECT_FALSE( MAC_Address{ lhs } != MAC_Address{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_TRUE( MAC_Address{ lhs } != MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator<( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random_unsigned_integer( 1 );
        auto const lhs = random_unsigned_integer( 0, rhs - 1 );

        EXPECT_TRUE( MAC_Address{ lhs } < MAC_Address{ rhs } );
    }

    {
        auto const rhs = random_unsigned_integer();
        auto const lhs = random_unsigned_integer( rhs );

        EXPECT_FALSE( MAC_Address{ lhs } < MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator>( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random_unsigned_integer( 1 );
        auto const rhs = random_unsigned_integer( 0, lhs - 1 );

        EXPECT_TRUE( MAC_Address{ lhs } > MAC_Address{ rhs } );
    }

    {
        auto const lhs = random_unsigned_integer();
        auto const rhs = random_unsigned_integer( lhs );

        EXPECT_FALSE( MAC_Address{ lhs } > MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator<=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random_unsigned_integer();
        auto const rhs = random_unsigned_integer( lhs );

        EXPECT_TRUE( MAC_Address{ lhs } <= MAC_Address{ rhs } );
    }

    {
        auto const lhs = random_unsigned_integer( 1 );
        auto const rhs = random_unsigned_integer( 0, lhs - 1 );

        EXPECT_FALSE( MAC_Address{ lhs } <= MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator>=( picolibrary::MAC_Address const &,
 *        picolibrary::MAC_Address const & ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random_unsigned_integer();
        auto const lhs = random_unsigned_integer( rhs );

        EXPECT_TRUE( MAC_Address{ lhs } >= MAC_Address{ rhs } );
    }

    {
        auto const rhs = random_unsigned_integer( 1 );
        auto const lhs = random_unsigned_integer( 0, rhs - 1 );

        EXPECT_FALSE( MAC_Address{ lhs } >= MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address> properly handles
 *        an invalid format string.
 */
TEST( outputFormatter, invalidFormatString )
{
    auto stream = Output_String_Stream{};

    auto const result = stream.print(
        ( std::string{ '{' } + random_container<std::string>( random<std::uint_fast8_t>( 1 ) ) + '}' )
            .c_str(),
        MAC_Address{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_FORMAT );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_TRUE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address> properly handles
 *        a print error.
 */
TEST( outputFormatter, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", MAC_Address{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address> works properly.
 */
TEST( outputFormatter, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const address = MAC_Address{ random_unsigned_integer() };

    EXPECT_FALSE( stream.print( "{}", address ).is_error() );

    EXPECT_EQ( stream.string(), hyphen_separated_hexadecimal_digit_pairs( address.as_byte_array() ) );
}

/**
 * \brief Execute the picolibrary::MAC_Address unit tests.
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
