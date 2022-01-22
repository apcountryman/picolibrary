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
 * \brief picolibrary::Format::Hexadecimal unit test program.
 */

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/format.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Format::Hexadecimal;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_format_string;
using ::testing::A;
using ::testing::Return;

template<typename Integer>
auto hexadecimal( Integer value )
{
    using U = std::make_unsigned_t<Integer>;

    auto const unsigned_value = [ value ]() {
        U u;
        static_assert( sizeof( u ) == sizeof( value ) );
        std::memcpy( &u, &value, sizeof( value ) );

        return u;
    }();

    auto stream = std::ostringstream{};

    stream << "0x" << std::hex << std::uppercase
           << std::setw( std::numeric_limits<U>::digits / 4 ) << std::setfill( '0' )
           << static_cast<std::uint_fast64_t>( unsigned_value );

    return stream.str();
}

} // namespace

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Hexadecimal> unit test fixture.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class outputFormatterHexadecimalDeathTest : public ::testing::Test {
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Hexadecimal> unit test fixture.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class outputFormatterHexadecimal : public ::testing::Test {
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Hexadecimal> unit test integer types.
 */
using Integers =
    ::testing::Types<std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t>;

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Hexadecimal> unit test fixture.
 */
TYPED_TEST_SUITE( outputFormatterHexadecimalDeathTest, Integers );

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Hexadecimal> unit test fixture.
 */
TYPED_TEST_SUITE( outputFormatterHexadecimal, Integers );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hexadecimal> properly
 *        handles an invalid format string.
 */
TYPED_TEST( outputFormatterHexadecimalDeathTest, invalidFormatString )
{
    using Integer = TypeParam;

    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( std::string{ '{' } + random_format_string( random<std::size_t>( 1, 15 ) ) + '}' )
                    .c_str(),
                Hexadecimal{ random<Integer>() } ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hexadecimal> properly
 *        handles a put error.
 */
TYPED_TEST( outputFormatterHexadecimal, putError )
{
    using Integer = TypeParam;

    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", Hexadecimal{ random<Integer>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Hexadecimal> works
 *        properly.
 */
TYPED_TEST( outputFormatterHexadecimal, worksProperly )
{
    using Integer = TypeParam;

    auto stream = Output_String_Stream{};

    auto const value = random<Integer>();

    auto const result = stream.print( "{}", Hexadecimal{ value } );

    ASSERT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), hexadecimal( value ) );
}

/**
 * \brief Execute the picolibrary::Format::Hexadecimal unit tests.
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
