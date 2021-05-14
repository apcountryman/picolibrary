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
 * \brief picolibrary::HSM::Event unit test program.
 */

#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/hsm.h"
#include "picolibrary/testing/unit/hsm.h"
#include "picolibrary/testing/unit/random.h"

namespace {

using ::picolibrary::HSM::Event_ID;
using ::picolibrary::Testing::Unit::random;
using ::picolibrary::Testing::Unit::random_container;
using ::picolibrary::Testing::Unit::HSM::Mock_Event_Category;
using ::testing::Return;

class Event final : public ::picolibrary::HSM::Event {
  public:
    Event() = delete;

    constexpr Event( ::picolibrary::HSM::Event_Category const & category, Event_ID id ) noexcept :
        ::picolibrary::HSM::Event{ category, id }
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
 * \brief Verify picolibrary::HSM::Event::Event( picolibrary::HSM::Event_Category const &,
 *        picolibrary::HSM::Event_ID ) works properly.
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
 * \brief Execute the picolibrary::HSM::Event unit tests.
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
