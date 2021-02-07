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
 * \brief picolibrary::Stream unit test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"
#include "picolibrary/utility.h"

namespace {

class Stream : public ::picolibrary::Stream {
  public:
    using ::picolibrary::Stream::Stream;

    ~Stream() noexcept = default;

    constexpr auto operator=( Stream && expression ) noexcept -> Stream & = default;

    constexpr auto operator=( Stream const & expression ) noexcept -> Stream & = default;

    using ::picolibrary::Stream::clear_end_of_file_reached_report;
    using ::picolibrary::Stream::report_end_of_file_reached;

    using ::picolibrary::Stream::clear_fatal_error;
    using ::picolibrary::Stream::report_fatal_error;

    using ::picolibrary::Stream::buffer;
    using ::picolibrary::Stream::set_buffer;
};

using ::picolibrary::Error_Code;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Stream_Buffer;
using ::picolibrary::Testing::Unit::random;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::Stream::Stream() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto stream = Stream{};

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
    EXPECT_FALSE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), nullptr );
}

/**
 * \brief Verify picolibrary::Stream::initialize() properly handles a device
 *        initialization error.
 */
TEST( initialize, deviceInitializationError )
{
    auto stream = Stream{};

    auto buffer = Mock_Stream_Buffer{};

    stream.set_buffer( &buffer );

    auto const error = random<Mock_Error>();

    EXPECT_CALL( buffer, initialize() ).WillOnce( Return( error ) );

    auto const result = stream.initialize();

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );
}

/**
 * \brief Verify picolibrary::Stream::initialize() works properly.
 */
TEST( initialize, worksProperly )
{
    auto stream = Stream{};

    auto buffer = Mock_Stream_Buffer{};

    stream.set_buffer( &buffer );

    EXPECT_CALL( buffer, initialize() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.initialize().is_error() );
}

/**
 * \brief Verify picolibrary::Stream::end_of_file_reached(),
 *        picolibrary::Stream::report_end_of_file_reached(), and
 *        picolibrary::Stream::clear_end_of_file_reached_report() work properly.
 */
TEST( endOfFileReached, worksProperly )
{
    auto stream = Stream{};

    stream.clear_end_of_file_reached_report();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.end_of_file_reached() );

    stream.report_end_of_file_reached();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_TRUE( stream.end_of_file_reached() );

    stream.report_end_of_file_reached();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_TRUE( stream.end_of_file_reached() );

    stream.clear_end_of_file_reached_report();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.end_of_file_reached() );
}

/**
 * \brief Verify picolibrary::Stream::io_error_present(),
 *        picolibrary::Stream::report_io_error(), and
 *        picolibrary::Stream::clear_io_error_report() work properly.
 */
TEST( ioError, worksProperly )
{
    auto stream = Stream{};

    stream.clear_io_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.io_error_present() );

    stream.report_io_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_FALSE( stream );
    EXPECT_TRUE( not stream );
    EXPECT_TRUE( stream.io_error_present() );

    stream.report_io_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_FALSE( stream );
    EXPECT_TRUE( not stream );
    EXPECT_TRUE( stream.io_error_present() );

    stream.clear_io_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.io_error_present() );
}

/**
 * \brief Verify picolibrary::fatal_error_present(),
 *        picolibrary::Stream::report_fatal_error(), and
 *        picolibrary::Stream::clear_fatal_error() work properly.
 */
TEST( fatalError, worksProperly )
{
    auto stream = Stream{};

    stream.clear_fatal_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.fatal_error_present() );

    stream.report_fatal_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_FALSE( stream );
    EXPECT_TRUE( not stream );
    EXPECT_TRUE( stream.fatal_error_present() );

    stream.report_fatal_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_FALSE( stream );
    EXPECT_TRUE( not stream );
    EXPECT_TRUE( stream.fatal_error_present() );

    stream.clear_fatal_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream );
    EXPECT_FALSE( not stream );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Stream::buffer(), picolibrary::Stream::buffer_is_set(), and
 *        picolibrary::Stream::set_buffer() work properly.
 */
TEST( buffer, worksProperly )
{
    auto stream = Stream{};

    auto buffer = Mock_Stream_Buffer{};

    stream.set_buffer( &buffer );

    EXPECT_TRUE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), &buffer );
}

/**
 * \brief Execute the picolibrary::Stream unit tests.
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
