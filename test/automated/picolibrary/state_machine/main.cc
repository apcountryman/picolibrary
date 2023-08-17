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
 * \brief picolibrary::State_Machine automated test program.
 */

#include <ostream>
#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/event.h"
#include "picolibrary/rom.h"
#include "picolibrary/state_machine.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Event_ID;
using ::picolibrary::Simple_Event;
using ::picolibrary::State_Machine;
using ::picolibrary::to_underlying;

class Test_State_Machine : public State_Machine {
  public:
    static Simple_Event const A;
    static Simple_Event const B;
    static Simple_Event const C;

    constexpr Test_State_Machine( std::ostream & stream ) noexcept :
        State_Machine{ initial_pseudostate },
        m_stream{ stream }
    {
    }

    Test_State_Machine( Test_State_Machine && ) = delete;

    Test_State_Machine( Test_State_Machine const & ) = delete;

    ~Test_State_Machine() noexcept = default;

    auto operator=( Test_State_Machine && ) = delete;

    auto operator=( Test_State_Machine const & ) = delete;

    void execute_initial_transition() noexcept
    {
        State_Machine::execute_initial_transition( INITIAL_TRANSITION_TRIGGER );
    }

    using State_Machine::dispatch;

  private:
    enum class Event : Event_ID {
        INITIAL_TRANSITION_TRIGGER,
        A,
        B,
        C,
    };

    class Event_Category final : public ::picolibrary::Event_Category {
      public:
        static constexpr auto instance() noexcept -> Event_Category const &
        {
            return INSTANCE;
        }

        Event_Category( Event_Category && ) = delete;

        Event_Category( Event_Category const & ) = delete;

        auto operator=( Event_Category && ) = delete;

        auto operator=( Event_Category const & ) = delete;

      private:
        static Event_Category const INSTANCE;

        constexpr Event_Category() noexcept = default;

        ~Event_Category() noexcept = default;

        auto name() const noexcept -> ::picolibrary::ROM::String override final
        {
            return PICOLIBRARY_ROM_STRING( "::Test_State_Machine::Event" );
        }

        auto event_description( Event_ID id ) const noexcept -> ::picolibrary::ROM::String override final
        {
            switch ( static_cast<Event>( id ) ) {
                    // clang-format off

                case Event::INITIAL_TRANSITION_TRIGGER: return PICOLIBRARY_ROM_STRING( "INITIAL_TRANSITION_TRIGGER" );
                case Event::A: return PICOLIBRARY_ROM_STRING( "A" );
                case Event::B: return PICOLIBRARY_ROM_STRING( "B" );
                case Event::C: return PICOLIBRARY_ROM_STRING( "C" );

                    // clang-format on
            } // switch

            return PICOLIBRARY_ROM_STRING( "UNKNOWN" );
        }
    };

    static Simple_Event const INITIAL_TRANSITION_TRIGGER;

    std::ostream & m_stream;

    static auto initial_pseudostate( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & test_state_machine = static_cast<Test_State_Machine &>( state_machine );

        test_state_machine.m_stream << "start-INIT;";

        return test_state_machine.transition_to( s1, event );
    }

    static auto s1( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & test_state_machine = static_cast<Test_State_Machine &>( state_machine );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_state_machine.m_stream << "s1-ENTRY;";
                    return test_state_machine.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_state_machine.m_stream << "s1-EXIT;";
                    return test_state_machine.event_handled( event );
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    test_state_machine.m_stream << "s1-A;";
                    return test_state_machine.event_handled( event );
                case Event::B:
                    test_state_machine.m_stream << "s1-B;";
                    return test_state_machine.transition_to( s1, event );
                case Event::C:
                    test_state_machine.m_stream << "s1-C;";
                    return test_state_machine.transition_to( s2, event );
                default: break;
            } // switch
        }     // if

        return test_state_machine.event_handled( event );
    }

    static auto s2( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & test_state_machine = static_cast<Test_State_Machine &>( state_machine );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_state_machine.m_stream << "s2-ENTRY;";
                    return test_state_machine.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_state_machine.m_stream << "s2-EXIT;";
                    return test_state_machine.event_handled( event );
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    test_state_machine.m_stream << "s2-A;";
                    return test_state_machine.event_handled( event );
                case Event::B:
                    test_state_machine.m_stream << "s2-B;";
                    return test_state_machine.transition_to( s2, event );
                case Event::C:
                    test_state_machine.m_stream << "s2-C;";
                    return test_state_machine.transition_to( s1, event );
                default: break;
            } // switch
        }     // if

        return test_state_machine.event_handled( event );
    }
};

Test_State_Machine::Event_Category const Test_State_Machine::Event_Category::INSTANCE{};

Simple_Event const Test_State_Machine::INITIAL_TRANSITION_TRIGGER{
    Event_Category::instance(),
    to_underlying( Event::INITIAL_TRANSITION_TRIGGER )
};

Simple_Event const Test_State_Machine::A{ Event_Category::instance(), to_underlying( Event::A ) };
Simple_Event const Test_State_Machine::B{ Event_Category::instance(), to_underlying( Event::B ) };
Simple_Event const Test_State_Machine::C{ Event_Category::instance(), to_underlying( Event::C ) };

} // namespace

/**
 * \brief Verify picolibrary::State_Machine works properly.
 */
TEST( stateMachine, worksProperly )
{
    auto stream = std::ostringstream{};

    auto test_state_machine = Test_State_Machine{ stream };

    test_state_machine.execute_initial_transition();
    stream << '\n';

    Simple_Event const event_sequence[]{
        // clang-format off

        Test_State_Machine::A,
        Test_State_Machine::B,
        Test_State_Machine::C,
        Test_State_Machine::A,
        Test_State_Machine::B,
        Test_State_Machine::C,

        // clang-format on
    };

    for ( auto const & event : event_sequence ) {
        test_state_machine.dispatch( event );
        stream << '\n';
    } // for

    EXPECT_EQ(
        stream.str(),
        "start-INIT;s1-ENTRY;\n"
        "s1-A;\n"
        "s1-B;s1-EXIT;s1-ENTRY;\n"
        "s1-C;s1-EXIT;s2-ENTRY;\n"
        "s2-A;\n"
        "s2-B;s2-EXIT;s2-ENTRY;\n"
        "s2-C;s2-EXIT;s1-ENTRY;\n" );
}

/**
 * \brief Execute the picolibrary::State_Machine automated tests.
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
