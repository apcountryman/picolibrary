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
 * \brief picolibrary::IP::TCP::Port unit test program.
 */

#include <sstream>
#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/ip/tcp.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::IP::TCP::Port;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Return;

auto random_unique_unsigned_integers()
{
    auto const a = random<Port::Unsigned_Integer>();
    auto const b = random<Port::Unsigned_Integer>();

    return std::pair<Port::Unsigned_Integer, Port::Unsigned_Integer>{
        a, b != a ? b : b ^ random<Port::Unsigned_Integer>( 1 )
    };
}

auto decimal( Port::Unsigned_Integer unsigned_integer )
{
    auto stream = std::ostringstream{};

    stream << unsigned_integer;

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::IP::TCP::Port::any() works properly.
 */
TEST( any, worksProperly )
{
    auto const port = Port::any();

    EXPECT_TRUE( port.is_any() );
    EXPECT_EQ( port.as_unsigned_integer(), 0 );
}

/**
 * \brief Verify picolibrary::IP::TCP::Port::Port() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const port = Port{};

    EXPECT_TRUE( port.is_any() );
    EXPECT_EQ( port.as_unsigned_integer(), 0 );
}

/**
 * \brief Verify picolibrary::IP::TCP::Port::Port(
 *        picolibrary::IP::TCP::Port::Unsigned_Integer ) works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    auto const unsigned_integer = random<Port::Unsigned_Integer>();

    auto const port = Port{ unsigned_integer };

    EXPECT_EQ( port.is_any(), unsigned_integer == 0 );
    EXPECT_EQ( port.as_unsigned_integer(), unsigned_integer );
}

/**
 * \brief Verify picolibrary::IP::TCP::operator==( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_TRUE( Port{ lhs } == Port{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_FALSE( Port{ lhs } == Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IP::TCP::operator!=( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_FALSE( Port{ lhs } != Port{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_unsigned_integers();

        EXPECT_TRUE( Port{ lhs } != Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IP::TCP::operator<( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random<Port::Unsigned_Integer>( 1 );
        auto const lhs = random<Port::Unsigned_Integer>( 0, rhs - 1 );

        EXPECT_TRUE( Port{ lhs } < Port{ rhs } );
    }

    {
        auto const rhs = random<Port::Unsigned_Integer>();
        auto const lhs = random<Port::Unsigned_Integer>( rhs );

        EXPECT_FALSE( Port{ lhs } < Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IP::TCP::operator>( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>( 1 );
        auto const rhs = random<Port::Unsigned_Integer>( 0, lhs - 1 );

        EXPECT_TRUE( Port{ lhs } > Port{ rhs } );
    }

    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = random<Port::Unsigned_Integer>( lhs );

        EXPECT_FALSE( Port{ lhs } > Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IP::TCP::operator<=( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = random<Port::Unsigned_Integer>( lhs );

        EXPECT_TRUE( Port{ lhs } <= Port{ rhs } );
    }

    {
        auto const lhs = random<Port::Unsigned_Integer>( 1 );
        auto const rhs = random<Port::Unsigned_Integer>( 0, lhs - 1 );

        EXPECT_FALSE( Port{ lhs } <= Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::IP::TCP::operator>=( picolibrary::IP::TCP::Port,
 *        picolibrary::IP::TCP::Port ) works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random<Port::Unsigned_Integer>();
        auto const lhs = random<Port::Unsigned_Integer>( rhs );

        EXPECT_TRUE( Port{ lhs } >= Port{ rhs } );
    }

    {
        auto const rhs = random<Port::Unsigned_Integer>( 1 );
        auto const lhs = random<Port::Unsigned_Integer>( 0, rhs - 1 );

        EXPECT_FALSE( Port{ lhs } >= Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::TCP::Port> properly
 *        handles an invalid format string.
 */
TEST( outputFormatter, invalidFormatString )
{
    auto stream = Output_String_Stream{};

    auto const result = stream.print(
        ( std::string{ '{' } + random_container<std::string>( random<std::uint_fast8_t>( 1 ) ) + '}' )
            .c_str(),
        Port{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_FORMAT );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_TRUE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::TCP::Port> properly
 *        handles a print error.
 */
TEST( outputFormatter, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", Port{} );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::TCP::Port> works properly.
 */
TEST( outputFormatter, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const port = Port{ random<Port::Unsigned_Integer>() };

    EXPECT_FALSE( stream.print( "{}", port ).is_error() );

    EXPECT_EQ( stream.string(), decimal( port.as_unsigned_integer() ) );
}

/**
 * \brief Execute the picolibrary::IP::TCP::Port unit tests.
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
