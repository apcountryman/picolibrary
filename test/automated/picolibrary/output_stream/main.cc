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
 * \brief picolibrary::Output_Stream automated test program.
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/result.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/error.h"
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

    MOCK_METHOD( (Result<std::size_t>), print, (Output_Stream &, Foo const &), ( const ) );
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

    auto print( Output_Stream & stream, ::Foo const & foo ) const noexcept -> Result<std::size_t>
    {
        return m_mock_output_formatter->print( stream, foo );
    }

  private:
    ::Mock_Output_Formatter const * m_mock_output_formatter{};
};

/**
 * \brief Verify picolibrary::Output_Stream::put( char ) properly handles a put error.
 */
TEST( putCharErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 63 };

    EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( 'W' );

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

    auto const character = 'g';

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<char>( Eq( character ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( character ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const *, char const * ) properly
 *        handles a put error.
 */
TEST( putCharBlockErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 83 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const string = std::string_view{ "QoHCz0" };

    auto const result = stream.put( string.begin(), string.end() );

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

    auto const string = std::string_view{ "FDBVjQgD" };

    EXPECT_CALL( stream.buffer(), put( std::string{ string } ) ).WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( string.begin(), string.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( char const * ) properly handles a put
 *        error.
 */
TEST( putNullTerminatedStringErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 169 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const string = "du1JWCGcsfXR3";

    auto const result = stream.put( string );

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

    auto const string = "UStwlrJdq";

    EXPECT_CALL( stream.buffer(), put( string ) ).WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( string ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t ) properly handles a put
 *        error.
 */
TEST( putUnsignedByteErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 9 };

    EXPECT_CALL( stream.buffer(), put( A<std::uint8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( std::uint8_t{ 0xB6 } );

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

    auto const value = std::uint8_t{ 0xFB };

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::uint8_t>( Eq( value ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::uint8_t const *, std::uint8_t const
 *        * ) properly handles a put error.
 */
TEST( putUnsignedByteBlockErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 101 };

    EXPECT_CALL( stream.buffer(), put( A<std::vector<std::uint8_t>>() ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::uint8_t>{ 0x04, 0x35, 0x13, 0x53 };

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

    auto const values = std::vector<std::uint8_t>{ 0x49, 0x70, 0xB9, 0x97, 0x02 };

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t ) properly handles a put
 *        error.
 */
TEST( putSignedByteErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 31 };

    EXPECT_CALL( stream.buffer(), put( A<std::int8_t>() ) ).WillOnce( Return( error ) );

    auto const result = stream.put( std::int8_t{ 0x3D } );

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

    auto const value = std::int8_t{ 0x12 };

    EXPECT_CALL( stream.buffer(), put( SafeMatcherCast<std::int8_t>( Eq( value ) ) ) )
        .WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( value ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::put( std::int8_t const *, std::int8_t const *
 *        ) properly handles a put error.
 */
TEST( putSignedByteBlockErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 101 };

    EXPECT_CALL( stream.buffer(), put( A<std::vector<std::int8_t>>() ) ).WillOnce( Return( error ) );

    auto const values = std::vector<std::int8_t>{ 0x09, 0x32, 0x75 };

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

    auto const values = std::vector<std::int8_t>{ 0x57, 0x22, 0x19, 0x54, 0x56, 0x4D };

    EXPECT_CALL( stream.buffer(), put( values ) ).WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.put( &*values.begin(), &*values.end() ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() properly handles a
 *        picolibrary::Output_Formatter::print() error.
 */
TEST( printErrorHandling, outputFormatterPrintError )
{
    auto stream = Mock_Output_Stream{};

    auto const formatter = Mock_Output_Formatter{};

    auto const error = Mock_Error{ 242 };

    EXPECT_CALL( formatter, print( _, _ ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Foo{ 89 }, Output_Formatter<Foo>{ formatter } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() works properly when there is a single
 *        value to format.
 */
TEST( print, worksProperlySingleValue )
{
    auto stream = Mock_Output_Stream{};

    auto const foo           = Foo{ 243 };
    auto const foo_formatter = Mock_Output_Formatter{};
    auto const foo_size      = std::size_t{ 156 };

    EXPECT_CALL( foo_formatter, print( Ref( stream ), Ref( foo ) ) ).WillOnce( Return( foo_size ) );

    auto const result = stream.print( foo, Output_Formatter<Foo>{ foo_formatter } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), foo_size );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::print() works properly when there are
 *        multiple values to format.
 */
TEST( print, worksProperlyMultipleValues )
{
    auto const in_sequence = InSequence{};

    auto stream = Mock_Output_Stream{};

    auto const foo_a           = Foo{ 201 };
    auto const foo_a_formatter = Mock_Output_Formatter{};
    auto const foo_a_size      = std::size_t{ 47 };
    auto const foo_b           = Foo{ 141 };
    auto const foo_b_formatter = Mock_Output_Formatter{};
    auto const foo_b_size      = std::size_t{ 253 };

    EXPECT_CALL( foo_a_formatter, print( Ref( stream ), Ref( foo_a ) ) ).WillOnce( Return( foo_a_size ) );
    EXPECT_CALL( foo_b_formatter, print( Ref( stream ), Ref( foo_b ) ) ).WillOnce( Return( foo_b_size ) );

    auto const result = stream.print(
        foo_a, Output_Formatter<Foo>{ foo_a_formatter }, foo_b, Output_Formatter<Foo>{ foo_b_formatter } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), foo_a_size + foo_b_size );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Stream::flush() properly handles a flush error.
 */
TEST( flushErrorHandling, flushError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 33 };

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

    EXPECT_CALL( stream.buffer(), flush() ).WillOnce( Return( Result<void>{} ) );

    EXPECT_FALSE( stream.flush().is_error() );

    EXPECT_TRUE( stream.is_nominal() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char>::print( picolibrary::Output_Stream &,
 *        char ) properly handles a put error.
 */
TEST( outputFormatterCharPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 181 };

    EXPECT_CALL( stream.buffer(), put( A<char>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( 'g' );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char>::print( picolibrary::Output_Stream &,
 *        char ) works properly.
 */
TEST( outputFormatterCharPrintOutputStream, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const character = 'u';

    auto const result = stream.print( character );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ character } );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *>::print(
 *        picolibrary::Output_Stream &, char const * ) properly handles a put error.
 */
TEST( outputFormatterNullTerminatedStringPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 105 };

    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "FxYCgTqc4" );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<char const *>::print(
 *        picolibrary::Output_Stream &, char const * ) works properly.
 */
TEST( outputFormatterNullTerminatedStringPrintOutputStream, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const string = "Ku67TKN3M5ITORA";

    auto const result = stream.print( string );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), string );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Error_Code>::print(
 *        picolibrary::Output_Stream &, picolibrary::Error_Code const & ) properly handles
 *        a put error.
 */
TEST( outputFormatterErrorCodePrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const error = Mock_Error{ 163 };

    EXPECT_CALL( Mock_Error_Category::instance(), name() )
        .WillOnce( Return( "pvtwn8xRCN9" ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( _ ) )
        .WillOnce( Return( "1aL94J2UIA" ) );
    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );

    auto const result = stream.print( Mock_Error{ 150 } );

    ASSERT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Error_Code>::print(
 *        picolibrary::Output_Stream &, picolibrary::Error_Code const & ) works properly
 *        with a picolibrary::Error_Code.
 */
TEST( outputFormatterErrorCodePrintOutputStream, worksProperlyErrorCode )
{
    auto stream = Output_String_Stream{};

    auto const error               = Mock_Error{ 120 };
    auto const error_category_name = "CjPf5bhQgbshej";
    auto const error_description   = "4snpgrnA4";

    EXPECT_CALL( Mock_Error_Category::instance(), name() ).WillOnce( Return( error_category_name ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( to_underlying( error ) ) )
        .WillOnce( Return( error_description ) );

    auto const result = stream.print( Error_Code{ error } );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ error_category_name } + "::" + error_description );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Error_Code>::print(
 *        picolibrary::Output_Stream &, picolibrary::Error_Code const & ) works properly
 *        with an error code enum.
 */
TEST( outputFormatterErrorCodePrintOutputStream, worksProperlyErrorCodeEnum )
{
    auto stream = Output_String_Stream{};

    auto const error               = Mock_Error{ 116 };
    auto const error_category_name = "68vDl0jKy";
    auto const error_description   = "McNFWXoDC36ZcSt";

    EXPECT_CALL( Mock_Error_Category::instance(), name() ).WillOnce( Return( error_category_name ) );
    EXPECT_CALL( Mock_Error_Category::instance(), error_description( to_underlying( error ) ) )
        .WillOnce( Return( error_description ) );

    auto const result = stream.print( error );

    ASSERT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ error_category_name } + "::" + error_description );
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
