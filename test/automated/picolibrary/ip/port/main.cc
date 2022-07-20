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
 * \brief picolibrary::IP::Port automated test program.
 */

#include <sstream>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/ip.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::IP::Port;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_unique_pair;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;

auto decimal( Port::Unsigned_Integer unsigned_integer ) -> std::string
{
    auto stream = std::ostringstream{};

    stream << unsigned_integer;

    return stream.str();
}

} // namespace

/**
 * \brief Verify picolibrary::IP::Port::Port() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto const port = Port{};

    EXPECT_TRUE( port.is_any() );
    EXPECT_EQ( port.as_unsigned_integer(), 0 );
}

/**
 * \brief Verify picolibrary::IP::Port::Port( picolibrary::IP::Port::Unsigned_Integer )
 *        works properly.
 */
TEST( constructorUnsignedInteger, worksProperly )
{
    {
        auto const unsigned_integer = Port::Unsigned_Integer{ 0 };

        auto const port = Port{ unsigned_integer };

        EXPECT_TRUE( port.is_any() );
        EXPECT_EQ( port.as_unsigned_integer(), unsigned_integer );
    }

    {
        auto const unsigned_integer = random<Port::Unsigned_Integer>( 1 );

        auto const port = Port{ unsigned_integer };

        EXPECT_FALSE( port.is_any() );
        EXPECT_EQ( port.as_unsigned_integer(), unsigned_integer );
    }
}

/**
 * \brief Verify picolibrary::operator==( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( equalityOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_TRUE( Port{ lhs } == Port{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_pair<Port::Unsigned_Integer>();

        EXPECT_FALSE( Port{ lhs } == Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator!=( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( inequalityOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = lhs;

        EXPECT_FALSE( Port{ lhs } != Port{ rhs } );
    }

    {
        auto const [ lhs, rhs ] = random_unique_pair<Port::Unsigned_Integer>();

        EXPECT_TRUE( Port{ lhs } != Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator<( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( lessThanOperator, worksProperly )
{
    {
        auto const rhs = random<Port::Unsigned_Integer>( 0 + 1 );
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
 * \brief Verify picolibrary::operator>( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( greaterThanOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>( 0 + 1 );
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
 * \brief Verify picolibrary::operator<=( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( lessThanOrEqualToOperator, worksProperly )
{
    {
        auto const lhs = random<Port::Unsigned_Integer>();
        auto const rhs = random<Port::Unsigned_Integer>( lhs );

        EXPECT_TRUE( Port{ lhs } <= Port{ rhs } );
    }

    {
        auto const lhs = random<Port::Unsigned_Integer>( 0 + 1 );
        auto const rhs = random<Port::Unsigned_Integer>( 0, lhs - 1 );

        EXPECT_FALSE( Port{ lhs } <= Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::operator>=( picolibrary::IP::Port, picolibrary::IP::Port )
 *        works properly.
 */
TEST( greaterThanOrEqualToOperator, worksProperly )
{
    {
        auto const rhs = random<Port::Unsigned_Integer>();
        auto const lhs = random<Port::Unsigned_Integer>( rhs );

        EXPECT_TRUE( Port{ lhs } >= Port{ rhs } );
    }

    {
        auto const rhs = random<Port::Unsigned_Integer>( 0 + 1 );
        auto const lhs = random<Port::Unsigned_Integer>( 0, rhs - 1 );

        EXPECT_FALSE( Port{ lhs } >= Port{ rhs } );
    }
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Port> properly handles a
 *        print error.
 */
TEST( outputFormatterIPPort, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Port{ random<Port::Unsigned_Integer>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::IP::Port> works properly.
 */
TEST( outputFormatterIPPort, worksProperly )
{
    {
        auto stream = Output_String_Stream{};

        auto const unsigned_integer = random<Port::Unsigned_Integer>();

        auto const port = Port{ unsigned_integer };

        auto const result = stream.print( port );

        ASSERT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), decimal( unsigned_integer ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const unsigned_integer = random<Port::Unsigned_Integer>();

        auto const port = Port{ unsigned_integer };

        auto const n = stream.print( port );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), decimal( unsigned_integer ) );
    }
}

/**
 * \brief Execute the picolibrary::IP::Port automated tests.
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
