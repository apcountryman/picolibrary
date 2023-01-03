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
 * \brief picolibrary::Output_Stream automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Output_Formatter;
using ::picolibrary::Output_Stream;
using ::picolibrary::Result;
using ::picolibrary::to_underlying;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Error_Category;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
using ::testing::_;
using ::testing::A;
using ::testing::Eq;
using ::testing::InSequence;
using ::testing::Ref;
using ::testing::Return;
using ::testing::SafeMatcherCast;

enum class Foo {};

class Mock_Output_Formatter {
  public:
    Mock_Output_Formatter() = default;

    Mock_Output_Formatter( Mock_Output_Formatter && ) = delete;

    Mock_Output_Formatter( Mock_Output_Formatter const & ) = delete;

    ~Mock_Output_Formatter() noexcept = default;

    auto operator=( Mock_Output_Formatter && ) = delete;

    auto operator=( Mock_Output_Formatter const & ) = delete;

    MOCK_METHOD( (Result<std::size_t, Error_Code>), print, (Output_Stream &, Foo const &), ( const ) );
};

} // namespace

template<>
class picolibrary::Output_Formatter<::Foo> {
  public:
    Output_Formatter() = delete;

    Output_Formatter( ::Mock_Output_Formatter const & mock_output_formatter ) noexcept :
        m_mock_output_formatter{ &mock_output_formatter }
    {
    }

    Output_Formatter( Output_Formatter && source ) noexcept = default;

    Output_Formatter( Output_Formatter const & original ) noexcept = default;

    ~Output_Formatter() noexcept = default;

    auto operator=( Output_Formatter && expression ) noexcept -> Output_Formatter & = default;

    auto operator=( Output_Formatter const & expression ) noexcept -> Output_Formatter & = default;

    auto print( Output_Stream & stream, ::Foo const & foo ) const noexcept
        -> Result<std::size_t, Error_Code>
    {
        return m_mock_output_formatter->print( stream, foo );
    }

  private:
    ::Mock_Output_Formatter const * m_mock_output_formatter{};
};

template<>
inline auto picolibrary::Testing::Automated::random<::Foo>() -> ::Foo
{
    return static_cast<::Foo>( random<std::underlying_type_t<::Foo>>() );
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
        .WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( character ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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

    EXPECT_CALL( stream.buffer(), put( string ) ).WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*string.begin(), &*string.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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

    EXPECT_CALL( stream.buffer(), put( string ) ).WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( string.c_str() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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
        .WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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
        .WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
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

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles a
 *        picolibrary::Output_Formatter::print() error.
 */
TEST( print, outputFormatterPrintError )
{
    auto stream = Mock_Output_Stream{};

    auto const formatter = Mock_Output_Formatter{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( formatter, print( _, _ ) ).WillOnce( Return( error ) );

    auto const result = stream.print( random<Foo>(), Output_Formatter<Foo>{ formatter } );

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
        auto stream = Mock_Output_Stream{};

        auto const foo           = random<Foo>();
        auto const foo_formatter = Mock_Output_Formatter{};
        auto const foo_size      = random<std::size_t>();

        EXPECT_CALL( foo_formatter, print( Ref( stream ), Ref( foo ) ) ).WillOnce( Return( foo_size ) );

        auto const result = stream.print( foo, Output_Formatter<Foo>{ foo_formatter } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), foo_size );

        EXPECT_TRUE( stream.is_nominal() );
    }

    {
        auto const in_sequence = InSequence{};

        auto stream = Mock_Output_Stream{};

        auto const foo_a           = random<Foo>();
        auto const foo_a_formatter = Mock_Output_Formatter{};
        auto const foo_a_size      = random<std::size_t>();
        auto const foo_b           = random<Foo>();
        auto const foo_b_formatter = Mock_Output_Formatter{};
        auto const foo_b_size      = random<std::size_t>();

        EXPECT_CALL( foo_a_formatter, print( Ref( stream ), Ref( foo_a ) ) ).WillOnce( Return( foo_a_size ) );
        EXPECT_CALL( foo_b_formatter, print( Ref( stream ), Ref( foo_b ) ) ).WillOnce( Return( foo_b_size ) );

        auto const result = stream.print(
            foo_a, Output_Formatter<Foo>{ foo_a_formatter }, foo_b, Output_Formatter<Foo>{ foo_b_formatter } );

        ASSERT_FALSE( result.is_error() );
        EXPECT_EQ( result.value(), foo_a_size + foo_b_size );

        EXPECT_TRUE( stream.is_nominal() );
    }
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

    EXPECT_CALL( stream.buffer(), flush() ).WillOnce( Return( Result<void, Error_Code>{} ) );

    EXPECT_FALSE( stream.flush().is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char> properly handles a put error.
 */
TEST( outputFormatterChar, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( random<char>() );

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

    auto const result = stream.print( character );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ character } );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *> properly handles a put error.
 */
TEST( outputFormatterNullTerminatedString, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = random<Mock_Error>();

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( random_container<std::string>().c_str() );

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

    auto const result = stream.print( string.c_str() );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), string );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Error_Code> properly handles a
 *        put error.
 */
TEST( outputFormatterErrorCode, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error_category_name = random_container<std::string>();
    auto const error_description   = random_container<std::string>();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( Mock_Error_Category::instance(), name() )
        .WillOnce( Return( error_category_name.c_str() ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( _ ) )
        .WillOnce( Return( error_description.c_str() ) );
    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Error_Code{ random<Mock_Error>() } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Error_Code> works properly.
 */
TEST( outputFormatterErrorCode, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const error               = random<Mock_Error>();
    auto const error_category_name = random_container<std::string>();
    auto const error_description   = random_container<std::string>();

    EXPECT_CALL( Mock_Error_Category::instance(), name() )
        .WillOnce( Return( error_category_name.c_str() ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( to_underlying( error ) ) )
        .WillOnce( Return( error_description.c_str() ) );

    auto const result = stream.print( Error_Code{ error } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), error_category_name + "::" + error_description );
}

/**
 * \brief Verify picolibrary::Output_Formatter<Enum,
 *        std::enable_if_t<picolibrary::is_error_code_enum_v<Enum>>> properly handles a
 *        put error.
 */
TEST( outputFormatterErrorCodeEnum, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error_category_name = random_container<std::string>();
    auto const error_description   = random_container<std::string>();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( Mock_Error_Category::instance(), name() )
        .WillOnce( Return( error_category_name.c_str() ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( _ ) )
        .WillOnce( Return( error_description.c_str() ) );
    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( random<Mock_Error>() );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<Enum,
 *        std::enable_if_t<picolibrary::is_error_code_enum_v<Enum>>> works properly.
 */
TEST( outputFormatterErrorCodeEnum, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const error               = random<Mock_Error>();
    auto const error_category_name = random_container<std::string>();
    auto const error_description   = random_container<std::string>();

    EXPECT_CALL( Mock_Error_Category::instance(), name() )
        .WillOnce( Return( error_category_name.c_str() ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( to_underlying( error ) ) )
        .WillOnce( Return( error_description.c_str() ) );

    auto const result = stream.print( error );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), error_category_name + "::" + error_description );
}

/**
 * \brief Execute the picolibrary::Output_Stream automated tests.
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
