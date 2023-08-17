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
 * \brief picolibrary::Event automated test program.
 */

#include <cstdint>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/event.h"
#include "picolibrary/stream.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/event.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Event;
using ::picolibrary::Event_Category;
using ::picolibrary::Event_ID;
using ::picolibrary::Output_Stream;
using ::picolibrary::Reliable_Output_Stream;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Event;
using ::picolibrary::Testing::Automated::Mock_Event_Category;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::Reliable_Output_String_Stream;
using ::testing::_;
using ::testing::A;
using ::testing::Matcher;
using ::testing::Ref;
using ::testing::Return;

} // namespace

/**
 * \brief Verify picolibrary::Event::Event( picolibrary::Event_Category const &,
 *        picolibrary::Event_ID ) works properly.
 */
TEST( constructor, worksProperly )
{
    auto const category = Mock_Event_Category{};
    auto const id       = Event_ID{ 90 };

    auto const event = Mock_Event{ category, id };

    EXPECT_EQ( &event.category(), &category );
    EXPECT_EQ( event.id(), id );

    auto const description = "jdmwJlCfF";

    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( description ) );

    EXPECT_STREQ( event.description(), description );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event>::print(
 *        picolibrary::Output_Stream &, picolibrary::Event const & ) properly handles a
 *        put error.
 */
TEST( outputFormatterEventPrintOutputStreamErrorHandling, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, 74 };

    auto const error = Mock_Error{ 73 };

    EXPECT_CALL( category, name() ).WillOnce( Return( "LXfJsA" ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( "H94qveeyD3j" ) );
    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( event, print_details( Matcher<Output_Stream &>( _ ) ) ).Times( 0 );

    auto const result = stream.print( static_cast<Event const &>( event ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event>::print(
 *        picolibrary::Output_Stream &, picolibrary::Event const & ) properly handles a
 *        details print error.
 */
TEST( outputFormatterEventPrintOutputStreamErrorHandling, detailsPrintError )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, 74 };

    auto const error = Mock_Error{ 196 };

    EXPECT_CALL( category, name() ).WillOnce( Return( "LXfJsA" ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( "H94qveeyD3j" ) );
    EXPECT_CALL( event, print_details( Matcher<Output_Stream &>( _ ) ) ).WillOnce( Return( error ) );

    auto const result = stream.print( static_cast<Event const &>( event ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event>::print(
 *        picolibrary::Output_Stream &, picolibrary::Event const & ) works properly.
 */
TEST( outputFormatterEventPrintOutputStream, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};
    auto const id       = Event_ID{ 114 };

    auto const event = Mock_Event{ category, id };

    auto const event_category_name = "VHhfhLG";
    auto const event_description   = "1maLI6tpGC";
    auto const event_details_size  = std::size_t{ 244 };

    EXPECT_CALL( category, name() ).WillOnce( Return( event_category_name ) );
    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( event_description ) );
    EXPECT_CALL( event, print_details( Matcher<Output_Stream &>( Ref( stream ) ) ) ).WillOnce( Return( event_details_size ) );

    auto const result = stream.print( static_cast<Event const &>( event ) );

    EXPECT_FALSE( result.is_error() );
    EXPECT_EQ( result.value(), stream.string().size() + event_details_size );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ event_category_name } + "::" + event_description );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event>::print(
 *        picolibrary::Reliable_Output_Stream &, picolibrary::Event const & ) works
 *        properly.
 */
TEST( outputFormatterEventPrintReliableOutputStream, worksProperly )
{
    auto stream = Reliable_Output_String_Stream{};

    auto const category = Mock_Event_Category{};
    auto const id       = Event_ID{ 114 };

    auto const event = Mock_Event{ category, id };

    auto const event_category_name = "VHhfhLG";
    auto const event_description   = "1maLI6tpGC";
    auto const event_details_size  = std::size_t{ 244 };

    EXPECT_CALL( category, name() ).WillOnce( Return( event_category_name ) );
    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( event_description ) );
    EXPECT_CALL( event, print_details( Matcher<Reliable_Output_Stream &>( Ref( stream ) ) ) )
        .WillOnce( Return( event_details_size ) );

    auto const n = stream.print( static_cast<Event const &>( event ) );

    EXPECT_EQ( n, stream.string().size() + event_details_size );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), std::string{ event_category_name } + "::" + event_description );
}

/**
 * \brief Execute the picolibrary::Event automated tests.
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
