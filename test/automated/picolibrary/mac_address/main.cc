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
 * \brief picolibrary::MAC_Address automated test program.
 */

#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/mac_address.h"
#include "picolibrary/precondition.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::BYPASS_PRECONDITION_EXPECTATION_CHECKS;
using ::picolibrary::MAC_Address;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_array;
using ::testing::A;
using ::testing::Return;

auto to_unsigned_integer( MAC_Address::Byte_Array const & byte_array ) -> MAC_Address::Unsigned_Integer
{
    // clang-format off

    return static_cast<MAC_Address::Unsigned_Integer>(
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 0 ] ) << 5 * std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 1 ] ) << 4 * std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 2 ] ) << 3 * std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 3 ] ) << 2 * std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 4 ] ) << 1 * std::numeric_limits<std::uint8_t>::digits ) |
        ( static_cast<MAC_Address::Unsigned_Integer>( byte_array[ 5 ] ) << 0 * std::numeric_limits<std::uint8_t>::digits ) );

    // clang-format on
}

auto random_unsigned_integer(
    MAC_Address::Unsigned_Integer min = UINT64_C( 0x00'00'00'00'00'00 ),
    MAC_Address::Unsigned_Integer max = UINT64_C( 0xFF'FF'FF'FF'FF'FF ) ) -> MAC_Address::Unsigned_Integer
{
    return random<MAC_Address::Unsigned_Integer>( min, max );
}

auto random_unique_unsigned_integer_pair()
{
    auto const a = random_unsigned_integer();
    auto const b = random_unsigned_integer();

    return std::pair<MAC_Address::Unsigned_Integer, MAC_Address::Unsigned_Integer>{
        a, b != a ? b : b ^ random_unsigned_integer( 1 )
    };
}

auto hyphen_separated_hexadecimal_digit_pairs( MAC_Address::Byte_Array const & byte_array ) -> std::string
{
    auto stream = std::ostringstream{};

    // clang-format off

    stream << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 0 ] ) << '-'
           << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 1 ] ) << '-'
           << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 2 ] ) << '-'
           << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 3 ] ) << '-'
           << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 4 ] ) << '-'
           << std::setw( 2 ) << std::setfill( '0' ) << std::hex << std::uppercase << static_cast<std::uint_fast16_t>( byte_array[ 5 ] );

    // clang-format on

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
    EXPECT_EQ( address.as_unsigned_integer(), 0x00'00'00'00'00'00 );
}

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Byte_Array const & ) works properly.
 */
TEST( constructorByteArray, worksProperly )
{
    struct {
        std::uint8_t first_octet;
        bool         is_locally_administered;
        bool         is_universally_administered;
        bool         is_multicast;
        bool         is_unicast;
    } const test_cases[]{
        // clang-format off

        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'0 ), false,  true, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'1 ), false,  true,  true, false },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'0 ),  true, false, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'1 ),  true, false,  true, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const byte_array = MAC_Address::Byte_Array{
            test_case.first_octet,  random<std::uint8_t>(), random<std::uint8_t>(),
            random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>(),
        };

        auto const address = MAC_Address{ byte_array };

        EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
        EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
        EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
        EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), to_unsigned_integer( byte_array ) );
    } // for
}

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    struct {
        std::uint8_t first_octet;
        bool         is_locally_administered;
        bool         is_universally_administered;
        bool         is_multicast;
        bool         is_unicast;
    } const test_cases[]{
        // clang-format off

        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'0 ), false,  true, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'1 ), false,  true,  true, false },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'0 ),  true, false, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'1 ),  true, false,  true, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const byte_array = MAC_Address::Byte_Array{
            test_case.first_octet,  random<std::uint8_t>(), random<std::uint8_t>(),
            random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>(),
        };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = MAC_Address{ unsigned_integer };

        EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
        EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
        EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
        EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    } // for
}

/**
 * \brief Verify picolibrary::MAC_Address::MAC_Address(
 *        picolibrary::Bypass_Precondition_Expectation_Checks,
 *        picolibrary::MAC_Address::Unsigned_Integer const & ) works properly.
 */
TEST( constructorBypassPreconditionExpectationChecksUnsignedInteger, worksProperly )
{
    struct {
        std::uint8_t first_octet;
        bool         is_locally_administered;
        bool         is_universally_administered;
        bool         is_multicast;
        bool         is_unicast;
    } const test_cases[]{
        // clang-format off

        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'0 ), false,  true, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'0'1 ), false,  true,  true, false },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'0 ),  true, false, false,  true },
        { static_cast<std::uint8_t>( ( random<std::uint8_t>() & 0b111111'0'0 ) | 0b000000'1'1 ),  true, false,  true, false },

        // clang-format on
    };

    for ( auto const test_case : test_cases ) {
        auto const byte_array = MAC_Address::Byte_Array{
            test_case.first_octet,  random<std::uint8_t>(), random<std::uint8_t>(),
            random<std::uint8_t>(), random<std::uint8_t>(), random<std::uint8_t>(),
        };
        auto const unsigned_integer = to_unsigned_integer( byte_array );

        auto const address = MAC_Address{ BYPASS_PRECONDITION_EXPECTATION_CHECKS, unsigned_integer };

        EXPECT_EQ( address.is_locally_administered(), test_case.is_locally_administered );
        EXPECT_EQ( address.is_universally_administered(), test_case.is_universally_administered );
        EXPECT_EQ( address.is_multicast(), test_case.is_multicast );
        EXPECT_EQ( address.is_unicast(), test_case.is_unicast );
        EXPECT_EQ( address.as_byte_array(), byte_array );
        EXPECT_EQ( address.as_unsigned_integer(), unsigned_integer );
    } // for
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
        auto const [ lhs, rhs ] = random_unique_unsigned_integer_pair();

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
        auto const [ lhs, rhs ] = random_unique_unsigned_integer_pair();

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
        auto const rhs = random_unsigned_integer( 0x00'00'00'00'00'00 + 1 );
        auto const lhs = random_unsigned_integer( 0x00'00'00'00'00'00, rhs - 1 );

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
        auto const lhs = random_unsigned_integer( 0x00'00'00'00'00'00 + 1 );
        auto const rhs = random_unsigned_integer( 0x00'00'00'00'00'00, lhs - 1 );

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
        auto const lhs = random_unsigned_integer( 0x00'00'00'00'00'00 + 1 );
        auto const rhs = random_unsigned_integer( 0x00'00'00'00'00'00, lhs - 1 );

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
        auto const rhs = random_unsigned_integer( 0x00'00'00'00'00'00 + 1 );
        auto const lhs = random_unsigned_integer( 0x00'00'00'00'00'00, rhs - 1 );

        EXPECT_FALSE( MAC_Address{ lhs } >= MAC_Address{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address> properly handles
 *        a put error.
 */
TEST( outputFormatterMACAddress, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( MAC_Address{ random_array<std::uint8_t, 6>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::MAC_Address> works properly.
 */
TEST( outputFormatterMACAddress, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const byte_array = random_array<std::uint8_t, 6>();

    auto const address = MAC_Address{ byte_array };

    auto const result = stream.print( address );

    ASSERT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), hyphen_separated_hexadecimal_digit_pairs( byte_array ) );
}

/**
 * \brief Execute the picolibrary::MAC_Address automated tests.
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
