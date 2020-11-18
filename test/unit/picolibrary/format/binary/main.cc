/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Format::Binary unit test program.
 */

#include <bitset>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/format.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"

namespace {

using ::picolibrary::Generic_Error;
using ::picolibrary::Format::Binary;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::A;
using ::testing::Return;

/**
 * \brief Format an integer.
 *
 * \tparam Integer The type of integer to format.
 *
 * \param[in] value The value to format.
 *
 * \return The formatted integer.
 */
template<typename Integer>
auto binary( Integer value )
{
    using U = std::make_unsigned_t<Integer>;

    auto const unsigned_value = [value]() {
        U u;
        static_assert( sizeof( value ) == sizeof( u ) );
        std::memcpy( &u, &value, sizeof( value ) );

        return u;
    }();

    auto stream = std::ostringstream{};

    stream << "0b" << std::bitset<std::numeric_limits<U>::digits>{ unsigned_value };

    return stream.str();
}

} // namespace

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Binary> unit test fixture.
 *
 * \tparam Integer The type of integer to be printed.
 */
template<typename Integer>
class outputFormatterBinary : public ::testing::Test {
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Binary> unit test integer types.
 */
using Integers =
    ::testing::Types<std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t>;

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Binary> unit test fixture.
 */
TYPED_TEST_SUITE( outputFormatterBinary, Integers );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Binary> properly handles an
 *        invalid format string.
 */
TYPED_TEST( outputFormatterBinary, invalidFormatString )
{
    using Integer = TypeParam;

    auto stream = Output_String_Stream{};

    auto const result = stream.print(
        ( std::string{ '{' } + random_container<std::string>( random<std::uint_fast8_t>( 1 ) ) + '}' )
            .c_str(),
        Binary{ random<Integer>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_FORMAT );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_TRUE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
    EXPECT_TRUE( stream.string().empty() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Binary> properly handles a
 *        print error.
 */
TYPED_TEST( outputFormatterBinary, printError )
{
    using Integer = TypeParam;

    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", Binary{ random<Integer>() } );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Binary> works properly.
 */
TYPED_TEST( outputFormatterBinary, worksProperly )
{
    using Integer = TypeParam;

    auto stream = Output_String_Stream{};

    auto const value = random<Integer>();

    EXPECT_FALSE( stream.print( "{}", Binary{ value } ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), binary( value ) );
}

/**
 * \brief Execute the picolibrary::Format::Binary unit tests.
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
