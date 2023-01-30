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
 * \brief picolibrary::Format::Dec automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <string>
#include <type_traits>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/format.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Format::Dec;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::A;
using ::testing::Return;

template<typename Integer>
auto dec( Integer value ) -> std::string
{
    auto stream = std::ostringstream{};

    stream << static_cast<
        std::conditional_t<std::is_same_v<Integer, std::int8_t>, std::int_fast16_t, std::conditional_t<std::is_same_v<Integer, std::uint8_t>, std::uint_fast16_t, Integer>>>(
        value );

    return stream.str();
}

} // namespace

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Dec> automated test fixture.
 *
 * \tparam Integer The type of integer to print.
 */
template<typename Integer>
class outputFormatterDec : public ::testing::Test {
};

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Dec> automated test integer types.
 */
using Integers =
    ::testing::Types<std::int8_t, std::uint8_t, std::int16_t, std::uint16_t, std::int32_t, std::uint32_t, std::int64_t, std::uint64_t>;

/**
 * \brief picolibrary::Output_Formatter<picolibrary::Dec> automated test fixture.
 */
TYPED_TEST_SUITE( outputFormatterDec, Integers );

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Dec> properly handles
 *        a put error.
 */
TYPED_TEST( outputFormatterDec, putError )
{
    using Integer = TypeParam;

    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Dec{ random<Integer>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Format::Dec> works properly.
 */
TYPED_TEST( outputFormatterDec, worksProperly )
{
    using Integer = TypeParam;

    {
        auto stream = Output_String_Stream{};

        auto const value = random<Integer>();

        auto const result = stream.print( Dec{ value } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dec( value ) );
    }

    {
        auto stream = Reliable_Output_String_Stream{};

        auto const value = random<Integer>();

        auto const n = stream.print( Dec{ value } );

        EXPECT_EQ( n, stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), dec( value ) );
    }
}

/**
 * \brief Execute the picolibrary::Format::Dec automated tests.
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
