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
 * \brief picolibrary::Reliable_Stream automated test program.
 */

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Testing::Automated::Mock_Reliable_Stream_Buffer;

class Reliable_Stream : public ::picolibrary::Reliable_Stream {
  public:
    using ::picolibrary::Reliable_Stream::Reliable_Stream;

    ~Reliable_Stream() noexcept = default;

    constexpr auto operator=( Reliable_Stream && expression ) noexcept -> Reliable_Stream & = default;

    constexpr auto operator  =( Reliable_Stream const & expression ) noexcept
        -> Reliable_Stream & = default;

    using ::picolibrary::Reliable_Stream::clear_end_of_file_reached_report;
    using ::picolibrary::Reliable_Stream::report_end_of_file_reached;

    using ::picolibrary::Reliable_Stream::buffer;
    using ::picolibrary::Reliable_Stream::set_buffer;
};

} // namespace

/**
 * \brief Verify picolibrary::Reliable_Stream::Reliable_Stream() works properly.
 */
TEST( constructorDefault, worksProperly )
{
    auto stream = Reliable_Stream{};

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_FALSE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), nullptr );
}

/**
 * \brief Verify picolibrary::Reliable_Stream::end_of_file_reached(),
 *        picolibrary::Reliable_Stream::report_end_of_file_reached(), and
 *        picolibrary::Reliable_Stream::clear_end_of_file_reached_report() work properly.
 */
TEST( endOfFileReached, worksProperly )
{
    auto stream = Reliable_Stream{};

    stream.clear_end_of_file_reached_report();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_FALSE( stream.end_of_file_reached() );

    stream.report_end_of_file_reached();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream.end_of_file_reached() );

    stream.report_end_of_file_reached();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_TRUE( stream.end_of_file_reached() );

    stream.clear_end_of_file_reached_report();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_FALSE( stream.end_of_file_reached() );
}

/**
 * \brief Verify picolibrary::Reliable_Stream::io_error_present(),
 *        picolibrary::Reliable_Stream::report_io_error(), and
 *        picolibrary::Reliable_Stream::clear_io_error() work properly.
 */
TEST( ioError, worksProperly )
{
    auto stream = Reliable_Stream{};

    stream.clear_io_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_FALSE( stream.io_error_present() );

    stream.report_io_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_TRUE( stream.io_error_present() );

    stream.report_io_error();

    EXPECT_FALSE( stream.is_nominal() );
    EXPECT_TRUE( stream.error_present() );
    EXPECT_TRUE( stream.io_error_present() );

    stream.clear_io_error();

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_FALSE( stream.error_present() );
    EXPECT_FALSE( stream.io_error_present() );
}

/**
 * \brief Verify picolibrary::Reliable_Stream::buffer_is_set(),
 *        picolibrary::Reliable_Stream::buffer(), and
 *        picolibrary::Reliable_Stream::set_buffer() work properly.
 */
TEST( buffer, worksProperly )
{
    auto stream = Reliable_Stream{};

    stream.set_buffer( nullptr );

    EXPECT_FALSE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), nullptr );

    auto buffer = Mock_Reliable_Stream_Buffer{};

    stream.set_buffer( &buffer );

    EXPECT_TRUE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), &buffer );

    stream.set_buffer( &buffer );

    EXPECT_TRUE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), &buffer );

    stream.set_buffer( nullptr );

    EXPECT_FALSE( stream.buffer_is_set() );
    EXPECT_EQ( stream.buffer(), nullptr );
}

/**
 * \brief Execute the picolibrary::Reliable_Stream automated tests.
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
