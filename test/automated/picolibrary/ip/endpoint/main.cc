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
 * \brief picolibrary::IP::Endpoint automated test program.
 */

#include <cstdint>
#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ip.h"
#include "picolibrary/ipv4.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/ip.h"
#include "picolibrary/testing/automated/ipv4.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IP::Address;
using ::picolibrary::IP::Endpoint;
using ::picolibrary::IP::Port;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;

using IPv4_Address = ::picolibrary::IPv4::Address;

auto unspecified_address_with_port( Port port ) -> std::string
{
    auto stream = std::ostringstream{};

    stream << "ANY:" << port.as_unsigned_integer();

    return stream.str();
}

auto dot_decimal_with_port( IPv4_Address const & address, Port port ) -> std::string
{
    auto stream = std::ostringstream{};

    stream << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 0 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 1 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 2 ] ) << '.'
           << static_cast<std::uint_fast16_t>( address.as_byte_array()[ 3 ] ) << ':'
           << port.as_unsigned_integer();

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::IP::Endpoint::Endpoint() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const endpoint = Endpoint{};

    EXPECT_EQ( endpoint.address(), Address{} );
    EXPECT_EQ( endpoint.port(), Port{} );
}

/**
 * \brief Verify picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Port ) works
 *        properly.
 */
TEST( constructorPort, worksProperly )
{
    auto const port = random<Port>();

    auto const endpoint = Endpoint{ port };

    EXPECT_EQ( endpoint.address(), Address{} );
    EXPECT_EQ( endpoint.port(), port );
}

/**
 * \brief Verify picolibrary::IP::Endpoint::Endpoint( picolibrary::IP::Address const &,
 *        picolibrary::IP::Port ) works properly.
 */
TEST( constructorAddressPort, worksProperly )
{
    auto const address = random<Address>();
    auto const port    = random<Port>();

    auto const endpoint = Endpoint{ address, port };

    EXPECT_EQ( endpoint.address(), address );
    EXPECT_EQ( endpoint.port(), port );
}

/**
 * \brief Verify picolibrary::IP::operator==( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        EXPECT_TRUE( Endpoint{} == Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } == Endpoint{ rhs_port }, lhs_port == rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } == Endpoint{ rhs_address, rhs_port } ),
            lhs_address == rhs_address and lhs_port == rhs_port );
    }
}

/**
 * \brief Verify picolibrary::IP::operator!=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        EXPECT_FALSE( Endpoint{} != Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } != Endpoint{ rhs_port }, lhs_port != rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } != Endpoint{ rhs_address, rhs_port } ),
            lhs_address != rhs_address or lhs_port != rhs_port );
    }
}

/**
 * \brief Verify picolibrary::IP::operator<( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        EXPECT_FALSE( Endpoint{} < Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } < Endpoint{ rhs_port }, lhs_port < rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } < Endpoint{ rhs_address, rhs_port } ),
            lhs_address < rhs_address or ( lhs_address == rhs_address and lhs_port < rhs_port ) );
    }
}

/**
 * \brief Verify picolibrary::IP::operator>( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        EXPECT_FALSE( Endpoint{} > Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } > Endpoint{ rhs_port }, lhs_port > rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } > Endpoint{ rhs_address, rhs_port } ),
            lhs_address > rhs_address or ( lhs_address == rhs_address and lhs_port > rhs_port ) );
    }
}

/**
 * \brief Verify picolibrary::IP::operator<=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        EXPECT_TRUE( Endpoint{} <= Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } <= Endpoint{ rhs_port }, lhs_port <= rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } <= Endpoint{ rhs_address, rhs_port } ),
            lhs_address < rhs_address or ( lhs_address == rhs_address and lhs_port <= rhs_port ) );
    }
}

/**
 * \brief Verify picolibrary::IP::operator>=( picolibrary::IP::Endpoint const &,
 *        picolibrary::IP::Endpoint const & ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        EXPECT_TRUE( Endpoint{} >= Endpoint{} );
    }

    {
        auto const lhs_port = random<Port>();
        auto const rhs_port = random<Port>();

        EXPECT_EQ( Endpoint{ lhs_port } >= Endpoint{ rhs_port }, lhs_port >= rhs_port );
    }

    {
        auto const lhs_address = random<Address>();
        auto const lhs_port    = random<Port>();
        auto const rhs_address = random<Address>();
        auto const rhs_port    = random<Port>();

        EXPECT_EQ(
            ( Endpoint{ lhs_address, lhs_port } >= Endpoint{ rhs_address, rhs_port } ),
            lhs_address > rhs_address or ( lhs_address == rhs_address and lhs_port >= rhs_port ) );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Endpoint> properly handles
 *        a print error.
 */
TEST( outputFormatterIPEndpoint, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Endpoint{ random<Address>(), random<Port>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Endpoint> works properly.
 */
TEST( outputFormatterIPEndpoint, worksProperly )
{
    {
        auto stream = Output_String_Stream{};

        auto const port = random<Port>();

        auto const result = stream.print( Endpoint{ port } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), unspecified_address_with_port( port ) );
    }

    {
        auto stream = Output_String_Stream{};

        auto const port = random<Port>();

        auto const result = stream.print( Endpoint{ IPv4_Address::any(), port } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), unspecified_address_with_port( port ) );
    }

    {
        auto stream = Output_String_Stream{};

        auto const ipv4_address = random<IPv4_Address>( { { 0, 0, 0, 1 } } );
        auto const port         = random<Port>();

        auto const result = stream.print( Endpoint{ ipv4_address, port } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dot_decimal_with_port( ipv4_address, port ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const port = random<Port>();

        auto const n = stream.print( Endpoint{ port } );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), unspecified_address_with_port( port ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const port = random<Port>();

        auto const n = stream.print( Endpoint{ IPv4_Address::any(), port } );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), unspecified_address_with_port( port ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const ipv4_address = random<IPv4_Address>( { { 0, 0, 0, 1 } } );
        auto const port         = random<Port>();

        auto const n = stream.print( Endpoint{ ipv4_address, port } );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dot_decimal_with_port( ipv4_address, port ) );
    }
}

/**
 * \brief Execute the picolibrary::IP::Endpoint automated tests.
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
