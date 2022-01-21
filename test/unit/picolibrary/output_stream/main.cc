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
 * \brief picolibrary::Output_Stream unit test program.
 */

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Generic_Error;
using ::picolibrary::Output_Stream;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::A;
using ::testing::Eq;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SafeMatcherCast;

auto random_format_string( std::size_t size = random<std::size_t>( 0, 15 ) )
{
    return random_container<std::string>( size, []() { return random<char>( ' ', 'z' ); } );
}

enum class Foo {};

class Mock_Output_Formatter {
  public:
    static auto & instance()
    {
        if ( not INSTANCE ) {
            throw std::logic_error{
                "::Mock_Output_Formatter::instance(): no active instance"
            };
        } // if

        return *INSTANCE;
    }

    Mock_Output_Formatter()
    {
        if ( INSTANCE ) {
            throw std::logic_error{
                "::Mock_Output_Formatter::Mock_Output_Formatter(): only one instance may "
                "be active at a time"
            };
        } // if

        INSTANCE = this;
    }

    ~Mock_Output_Formatter() noexcept
    {
        INSTANCE = nullptr;
    }

    Mock_Output_Formatter( Mock_Output_Formatter && ) = delete;

    Mock_Output_Formatter( Mock_Output_Formatter const & ) = delete;

    MOCK_METHOD( char const *, parse, ( std::string ) );

    MOCK_METHOD( (Result<std::size_t, Error_Code>), print, (Output_Stream &, Foo const &));

  private:
    inline static auto INSTANCE = static_cast<Mock_Output_Formatter *>( nullptr );
};

} // namespace

template<>
class picolibrary::Output_Formatter<::Foo> {
  public:
    constexpr Output_Formatter() noexcept = default;

    Output_Formatter( Output_Formatter && ) = delete;

    Output_Formatter( Output_Formatter const & ) = delete;

    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && ) = delete;

    auto operator=( Output_Formatter const & ) = delete;

    auto parse( char const * format ) noexcept
    {
        return ::Mock_Output_Formatter::instance().parse( format );
    }

    auto print( Output_Stream & stream, ::Foo const & foo ) noexcept
    {
        return ::Mock_Output_Formatter::instance().print( stream, foo );
    }
};

template<>
inline auto picolibrary::Testing::Unit::random<::Foo>()
{
    return static_cast<::Foo>( random<std::underlying_type_t<::Foo>>() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char ) properly handles the stream not
 *        being nominal.
 */
TEST( putCharDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<char>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<char>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<char>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char ) properly handles a put error.
 */
TEST( putChar, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( random<char>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char ) works properly.
 */
TEST( putChar, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const character = random<char>();

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<char>( Eq( character ) ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( character ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const *, char const * ) properly
 *        handles the stream not being nominal.
 */
TEST( putCharBlockDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            auto const string = random_container<std::string>();
            static_cast<void>( stream.put( &*string.begin(), &*string.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            auto const string = random_container<std::string>();
            static_cast<void>( stream.put( &*string.begin(), &*string.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            auto const string = random_container<std::string>();
            static_cast<void>( stream.put( &*string.begin(), &*string.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const *, char const * ) properly
 *        handles a put error.
 */
TEST( putCharBlock, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const string = random_container<std::string>();
    auto const result = stream.put( &*string.begin(), &*string.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const *, char const * ) works
 *        properly.
 */
TEST( putCharBlock, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const string = random_container<std::string>();

    EXPECT_CALL( stream.buffer(), put( string ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*string.begin(), &*string.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const * ) properly handles the
 *        stream not being nominal.
 */
TEST( putNullTerminatedStringDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            static_cast<void>( stream.put( random_container<std::string>().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            static_cast<void>( stream.put( random_container<std::string>().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).Times( 0 );

            static_cast<void>( stream.put( random_container<std::string>().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const * ) properly handles a put
 *        error.
 */
TEST( putNullTerminatedString, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const string = random_container<std::string>();
    auto const result = stream.put( string.c_str() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const * ) works properly.
 */
TEST( putNullTerminatedString, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const string = random_container<std::string>();

    EXPECT_CALL( stream.buffer(), put( string ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( string.c_str() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t ) properly handles the
 *        stream not being nominal.
 */
TEST( putUnsignedByteDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::uint8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::uint8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::uint8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t ) properly handles a put
 *        error.
 */
TEST( putUnsignedByte, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( random<std::uint8_t>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t ) works properly.
 */
TEST( putUnsignedByte, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const value = random<std::uint8_t>();

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( value ) ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t const *, std::uint8_t const
 *        * ) properly handles the stream not being nominal.
 */
TEST( putUnsignedByteBlockDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::uint8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::uint8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::uint8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::uint8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::uint8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::uint8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t const *, std::uint8_t const
 *        * ) properly handles a put error.
 */
TEST( putUnsignedByteBlock, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const values = random_container<std::vector<std::uint8_t>>();
    auto const result = stream.put( &*values.begin(), &*values.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t const *, std::uint8_t const
 *        * ) works properly.
 */
TEST( putUnsignedByteBlock, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const values = random_container<std::vector<std::uint8_t>>();

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t ) properly handles the
 *        stream not being nominal.
 */
TEST( putSignedByteDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::int8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::int8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::int8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::int8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::int8_t>() ) ).Times( 0 );

            static_cast<void>( stream.put( random<std::int8_t>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t ) properly handles a put
 *        error.
 */
TEST( putSignedByte, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::int8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( random<std::int8_t>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t ) works properly.
 */
TEST( putSignedByte, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const value = random<std::int8_t>();

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::int8_t>( Eq( value ) ) ) )
        .WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t const *, std::int8_t const *
 *        ) properly handles the stream not being nominal.
 */
TEST( putSignedByteBlockDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::int8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::int8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::int8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::int8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<std::vector<std::int8_t>>() ) ).Times( 0 );

            auto const values = random_container<std::vector<std::int8_t>>();
            static_cast<void>( stream.put( &*values.begin(), &*values.end() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t const *, std::int8_t const *
 *        ) properly handles a put error.
 */
TEST( putSignedByteBlock, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::vector<std::int8_t>>() ) ).WillOnce( Return( error ) );

    auto const values = random_container<std::vector<std::int8_t>>();
    auto const result = stream.put( &*values.begin(), &*values.end() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t const *, std::int8_t const *
 *        ) works properly.
 */
TEST( putSignedByteBlock, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    auto const values = random_container<std::vector<std::int8_t>>();

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles the stream not being
 *        nominal.
 */
TEST( printDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print( random_format_string().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print( random_format_string().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print( random_format_string().c_str() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print(
                ( random_format_string() + "{}" + random_format_string() ).c_str(), random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print(
                ( random_format_string() + "{}" + random_format_string() ).c_str(), random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), put( A<char>() ) ).Times( 0 );

            static_cast<void>( stream.print(
                ( random_format_string() + "{}" + random_format_string() ).c_str(), random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles a put error.
 */
TEST( print, putError )
{
    {
        auto stream = Mock_Output_Stream{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

        auto const result = stream.print(
            random_format_string( random<std::size_t>( 1, 15 ) ).c_str() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_FALSE( stream.end_of_file_reached() );
        EXPECT_FALSE( stream.io_error_present() );
        EXPECT_TRUE( stream.fatal_error_present() );
    }

    {
        auto stream = Mock_Output_Stream{};

        auto const error = random<Mock_Error>();

        EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

        auto const result = stream.print(
            ( random_format_string( random<std::size_t>( 1, 15 ) ) + "{}" + random_format_string() )
                .c_str(),
            random<Foo>() );

        ASSERT_TRUE( result.is_error() );
        EXPECT_EQ( result.error(), error );

        EXPECT_FALSE( stream.end_of_file_reached() );
        EXPECT_FALSE( stream.io_error_present() );
        EXPECT_TRUE( stream.fatal_error_present() );
    }
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles an invalid format
 *        string.
 */
TEST( printDeathTest, invalidFormatString )
{
    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( random_format_string() + '{' + random_format_string() ).c_str() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );

    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( random_format_string() + '}' + random_format_string() ).c_str() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );

    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( random_format_string() + '}' + random_format_string() + "{}" + random_format_string() )
                    .c_str(),
                random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );

    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                random_format_string().c_str(), random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );

    EXPECT_DEATH(
        {
            auto formatter = Mock_Output_Formatter{};

            auto const format = random_format_string();

            EXPECT_CALL( formatter, parse( _ ) ).WillOnce( Return( format.c_str() ) );

            static_cast<void>( Output_String_Stream{}.print(
                ( random_format_string() + '{' + random_format_string() + '}' + random_format_string() )
                    .c_str(),
                random<Foo>() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles a
 *        picolibrary::Output_Formatter::print() error.
 */
TEST( print, outputFormatterPrintError )
{
    auto stream = Output_String_Stream{};

    auto formatter = Mock_Output_Formatter{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( formatter, parse( _ ) ).WillOnce( Return( "}" ) );
    EXPECT_CALL( formatter, print( _, _ ) ).WillOnce( Return( error ) );

    auto const result = stream.print(
        ( random_format_string() + '{' + random_format_string() + '}' + random_format_string() )
            .c_str(),
        random<Foo>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() works properly.
 */
TEST( print, worksProperly )
{
    {
        auto stream = Output_String_Stream{};

        auto const a = random_format_string();
        auto const b = random_format_string();
        auto const c = random_format_string();

        auto const result = stream.print( ( a + "{{" + b + "}}" + c ).c_str() );

        ASSERT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), a + '{' + b + '}' + c );
    }

    {
        auto const in_sequence = InSequence{};

        auto stream = Output_String_Stream{};

        auto formatter = Mock_Output_Formatter{};

        auto const a = random_format_string();
        auto const b = random_format_string();
        auto const c = random_format_string();
        auto const d = random_format_string();
        auto const e = random_format_string();

        auto const format_specification_begin = d + '}' + e;
        auto const format_specification_end   = std::string{ '}' } + e;

        auto const foo      = random<Foo>();
        auto const foo_size = random<std::size_t>();

        EXPECT_CALL( formatter, parse( format_specification_begin ) )
            .WillOnce( Return( format_specification_end.c_str() ) );
        EXPECT_CALL( formatter, print( Ref( stream ), Ref( foo ) ) ).WillOnce( Return( foo_size ) );

        auto const result = stream.print(
            ( a + "{{" + b + "}}" + c + '{' + d + '}' + e ).c_str(), foo );

        ASSERT_TRUE( result.is_value() );
        EXPECT_EQ( result.value(), stream.string().size() );

        EXPECT_TRUE( stream.is_nominal() );
        EXPECT_EQ( stream.string(), a + '{' + b + '}' + c + e );
    }
}

/**
 * \brief Verify picolibrary::Output_Stream::flush() properly handles the stream not being
 *        nominal.
 */
TEST( flushDeathTest, notNominal )
{
    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_end_of_file_reached();

            EXPECT_CALL( stream.buffer(), flush() ).Times( 0 );

            static_cast<void>( stream.flush() );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_io_error();

            EXPECT_CALL( stream.buffer(), flush() ).Times( 0 );

            static_cast<void>( stream.flush() );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );

    EXPECT_DEATH(
        {
            auto stream = Mock_Output_Stream{};

            stream.report_fatal_error();

            EXPECT_CALL( stream.buffer(), flush() ).Times( 0 );

            static_cast<void>( stream.flush() );
        },
        "::picolibrary::Generic_Error::IO_STREAM_DEGRADED" );
}

/**
 * \brief Verify picolibrary::Output_Stream::flush() properly handles a flush error.
 */
TEST( flush, flushError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), flush() ).WillOnce( Return( error ) );

    auto const result = stream.flush();

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::flush() works properly.
 */
TEST( flush, worksProperly )
{
    auto stream = Mock_Output_Stream{};

    EXPECT_CALL( stream.buffer(), flush() ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.flush().is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char> properly handles an invalid format
 *        string.
 */
TEST( outputFormatterCharDeathTest, invalidFormatString )
{
    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( std::string{ '{' } + random_format_string( random<std::size_t>( 1, 15 ) ) + '}' )
                    .c_str(),
                random<char>() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char> properly handles a put error.
 */
TEST( outputFormatterChar, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", random<char>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char> works properly.
 */
TEST( outputFormatterChar, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const character = random<char>();

    auto const result = stream.print( "{}", character );

    ASSERT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ character } );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *> properly handles an invalid
 *        format string.
 */
TEST( outputFormatterNullTerminatedStringDeathTest, invalidFormatString )
{
    EXPECT_DEATH(
        {
            static_cast<void>( Output_String_Stream{}.print(
                ( std::string{ '{' } + random_format_string( random<std::size_t>( 1, 15 ) ) + '}' )
                    .c_str(),
                random_container<std::string>().c_str() ) );
        },
        "::picolibrary::Generic_Error::INVALID_FORMAT" );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *> properly handles a put error.
 */
TEST( outputFormatterNullTerminatedString, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", random_container<std::string>().c_str() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *> works properly.
 */
TEST( outputFormatterNullTerminatedString, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const string = random_container<std::string>();

    auto const result = stream.print( "{}", string.c_str() );

    ASSERT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), string );
}

/**
 * \brief Execute the picolibrary::Output_Stream unit tests.
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
