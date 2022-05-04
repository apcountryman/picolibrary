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
 * \brief picolibrary::Event automated test program.
 */

#include <cstdint>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/event.h"
#include "picolibrary/testing/automated/error.h"
#include "picolibrary/testing/automated/event.h"
#include "picolibrary/testing/automated/random.h"
#include "picolibrary/testing/automated/stream.h"

namespace {

using ::picolibrary::Event_Category;
using ::picolibrary::Event_ID;
using ::picolibrary::Testing::Automated::Mock_Error;
using ::picolibrary::Testing::Automated::Mock_Event;
using ::picolibrary::Testing::Automated::Mock_Event_Category;
using ::picolibrary::Testing::Automated::Mock_Output_Stream;
using ::picolibrary::Testing::Automated::Output_String_Stream;
using ::picolibrary::Testing::Automated::random;
using ::picolibrary::Testing::Automated::random_container;
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

    EXPECT_EQ( &event.category(), &category );
    EXPECT_EQ( event.id(), id );

    auto const description = random_container<std::string>();

    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( description.c_str() ) );

    EXPECT_STREQ( event.description(), description.c_str() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> properly handles a put
 *        error.
 */
TEST( outputFormatterEvent, putError )
{
    auto stream = Mock_Output_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, random<Event_ID>() };

    auto const event_category_name = random_container<std::string>();
    auto const event_description   = random_container<std::string>();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( category, name() ).WillOnce( Return( event_category_name.c_str() ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( event_description.c_str() ) );
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
TEST( outputFormatterEvent, detailsPrintError )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};

    auto const event = Mock_Event{ category, random<Event_ID>() };

    auto const event_category_name = random_container<std::string>();
    auto const event_description   = random_container<std::string>();

    auto const error = random<Mock_Error>();

    EXPECT_CALL( category, name() ).WillOnce( Return( event_category_name.c_str() ) );
    EXPECT_CALL( category, event_description( _ ) ).WillOnce( Return( event_description.c_str() ) );
    EXPECT_CALL( event, print_details( _ ) ).WillOnce( Return( error ) );

    auto const result = stream.print( "{}", static_cast<::picolibrary::Event const &>( event ) );

    EXPECT_TRUE( result.is_error() );
    EXPECT_EQ( result.error(), error );

    EXPECT_FALSE( stream.end_of_file_reached() );
    EXPECT_FALSE( stream.io_error_present() );
    EXPECT_TRUE( stream.fatal_error_present() );
}

/**
 * \brief Verify picolibrary::Output_Formatter<picolibrary::Event> works properly.
 */
TEST( outputFormatterEvent, worksProperly )
{
    auto stream = Output_String_Stream{};

    auto const category = Mock_Event_Category{};
    auto const id       = random<Event_ID>();

    auto const event = Mock_Event{ category, id };

    auto const event_category_name = random_container<std::string>();
    auto const event_description   = random_container<std::string>();
    auto const event_details_size  = random<std::size_t>();

    EXPECT_CALL( category, name() ).WillOnce( Return( event_category_name.c_str() ) );
    EXPECT_CALL( category, event_description( id ) ).WillOnce( Return( event_description.c_str() ) );
    EXPECT_CALL( event, print_details( Ref( stream ) ) ).WillOnce( Return( event_details_size ) );

    auto const result = stream.print( "{}", static_cast<::picolibrary::Event const &>( event ) );

    EXPECT_TRUE( result.is_value() );
    EXPECT_EQ( result.value(), stream.string().size() + event_details_size );

    EXPECT_TRUE( stream.is_nominal() );
    EXPECT_EQ( stream.string(), event_category_name + "::" + event_description );
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
