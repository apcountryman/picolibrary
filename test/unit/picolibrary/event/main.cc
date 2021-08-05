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
 * \brief picolibrary::Event unit test program.
 */

#include <cstdint>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/error.h"
#include "picolibrary/event.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/error.h"
#include "picolibrary/testing/unit/event.h"
#include "picolibrary/testing/unit/random.h"
#include "picolibrary/testing/unit/stream.h"
#include "picolibrary/void.h"

namespace {

using ::picolibrary::Error_Code;
using ::picolibrary::Event_Category;
using ::picolibrary::Event_ID;
using ::picolibrary::Generic_Error;
using ::picolibrary::Result;
using ::picolibrary::Void;
using ::picolibrary::Testing::Unit::Mock_Error;
using ::picolibrary::Testing::Unit::Mock_Event;
using ::picolibrary::Testing::Unit::Mock_Event_Category;
using ::picolibrary::Testing::Unit::Mock_Output_Stream;
using ::picolibrary::Testing::Unit::Output_String_Stream;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::testing::_;
using ::testing::A;
using ::testing::Ref;
using ::testing::Return;

class Event final : public ::picolibrary::Event {
  public:
    Event() = delete;

    constexpr Event( Event_Category const & category, Event_ID id ) noexcept :
        ::picolibrary::Event{ category, id }
    {
    }

    constexpr Event( Event && source ) noexcept = default;

    constexpr Event( Event const & original ) noexcept = default;

    virtual ~Event() noexcept override final = default;

    constexpr auto operator=( Event && expression ) noexcept -> Event & = default;

    constexpr auto operator=( Event const & expression ) noexcept -> Event & = default;
};

} // namespace

/**
 * \brief Verify picolibrary::Event::Event( picolibrary::Event_Category const &,
 *        picolibrary::Event_ID ) works properly.
 */
TEST( constructor, worksProperly )
{
    auto const category = Mock_Event_Category{};
    auto const id       = random<Event_ID>();

    auto const event = Event{ category, id };

    auto const description = random_container<std::string>();

    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( description.c_str() ) );

    EXPECT_EQ( &event.category(), &category );
    EXPECT_EQ( event.id(), id );
    EXPECT_STREQ( event.description(), description.c_str() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> properly handles an
 *        invalid format string.
 */
TEST( outputFormatter, invalidFormatString )
{
    auto stream = Output_String_Stream{};

    auto const result = stream.print(
        ( std::string{ '{' } + random_container<std::string>( random<std::uint_fast8_t>( 1 ) ) + '}' )
            .c_str(),
        static_cast<::picolibrary::Event const &>( Mock_Event{} ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), Generic_Error::INVALID_FORMAT );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_TRUE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> properly handles a
 *        print error
 */
TEST( outputFormatter, printError )
{
    auto stream = Mock_Output_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, random<Event_ID>() };

    auto const category_name = random_container<std::string>();
    auto const description   = random_container<std::string>();
    auto const error         = random<Mock_Error>();

    EXPECT_CALL( category, name() ).WillOnce( Return( category_name.c_str() ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( description.c_str() ) );
    EXPECT_CALL( stream.buffer(), put( A<std::string>() ) ).WillOnce( Return( error ) );
    EXPECT_CALL( event, print_details( _ ) ).Times( 0 );

    auto const result = stream.print( "{}", static_cast<::picolibrary::Event const &>( event ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> properly handles a
 *        details print error.
 */
TEST( outputFormatter, detailsPrintError )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, random<Event_ID>() };

    auto const category_name = random_container<std::string>();
    auto const description   = random_container<std::string>();
    auto const error         = random<Mock_Error>();

    EXPECT_CALL( category, name() ).WillOnce( Return( category_name.c_str() ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( description.c_str() ) );
    EXPECT_CALL( event, print_details( _ ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", static_cast<::picolibrary::Event const &>( event ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_FALSE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> works properly.
 */
TEST( outputFormatter, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};
    auto const id       = random<Event_ID>();

    auto const event = Mock_Event{ category, id };

    auto const category_name = random_container<std::string>();
    auto const description   = random_container<std::string>();

    EXPECT_CALL( category, name() ).WillOnce( Return( category_name.c_str() ) );
    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( description.c_str() ) );
    EXPECT_CALL( event, print_details( Ref( stream ) ) ).WillOnce( Return( Result<Void, Error_Code>{} ) );

    EXPECT_FALSE( stream.print( "{}", static_cast<::picolibrary::Event const &>( event ) ).is_error() );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), category_name + "::" + description );
}

/**
 * \brief Execute the picolibrary::Event unit tests.
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
