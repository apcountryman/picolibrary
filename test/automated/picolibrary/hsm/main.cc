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
 * \brief picolibrary::HSM automated test program.
 */

#include <ostream>
#include <sstream>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "picolibrary/event.h"
#include "picolibrary/hsm.h"
#include "picolibrary/rom.h"
#include "picolibrary/utility.h"

namespace {

using ::picolibrary::Event_ID;
using ::picolibrary::HSM;
using ::picolibrary::Simple_Event;
using ::picolibrary::to_underlying;

class Test_HSM : public HSM {
  public:
    static Simple_Event const A;
    static Simple_Event const B;
    static Simple_Event const C;
    static Simple_Event const D;
    static Simple_Event const E;
    static Simple_Event const F;
    static Simple_Event const G;
    static Simple_Event const H;
    static Simple_Event const I;

    constexpr Test_HSM( std::ostream & stream ) noexcept :
        HSM{ initial_pseudostate },
        m_stream{ stream }
    {
    }

    Test_HSM( Test_HSM && ) = delete;

    Test_HSM( Test_HSM const & ) = delete;

    ~Test_HSM() noexcept = default;

    auto operator=( Test_HSM && ) = delete;

    auto operator=( Test_HSM const & ) = delete;

    void execute_topmost_initial_transition() noexcept
    {
        HSM::execute_topmost_initial_transition( TOPMOST_INITIAL_TRANSITION_TRIGGER );
    }

    using HSM::dispatch;

  private:
    enum class Event : Event_ID {
        TOPMOST_INITIAL_TRANSITION_TRIGGER,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
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
            return PICOLIBRARY_ROM_STRING( "::Test_HSM::Event" );
        }

        auto event_description( Event_ID id ) const noexcept -> ::picolibrary::ROM::String override final
        {
            switch ( static_cast<Event>( id ) ) {
                    // clang-format off

                case Event::TOPMOST_INITIAL_TRANSITION_TRIGGER: return PICOLIBRARY_ROM_STRING( "TOPMOST_INITIAL_TRANSITION_TRIGGER" );
                case Event::A: return PICOLIBRARY_ROM_STRING( "A" );
                case Event::B: return PICOLIBRARY_ROM_STRING( "B" );
                case Event::C: return PICOLIBRARY_ROM_STRING( "C" );
                case Event::D: return PICOLIBRARY_ROM_STRING( "D" );
                case Event::E: return PICOLIBRARY_ROM_STRING( "E" );
                case Event::F: return PICOLIBRARY_ROM_STRING( "F" );
                case Event::G: return PICOLIBRARY_ROM_STRING( "G" );
                case Event::H: return PICOLIBRARY_ROM_STRING( "H" );
                case Event::I: return PICOLIBRARY_ROM_STRING( "I" );

                    // clang-format on
            } // switch

            return PICOLIBRARY_ROM_STRING( "UNKNOWN" );
        }
    };

    static Simple_Event const TOPMOST_INITIAL_TRANSITION_TRIGGER;

    std::ostream & m_stream;

    bool m_foo{};

    static auto initial_pseudostate( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        test_hsm.m_foo = false;

        test_hsm.m_stream << "top-INIT;";

        return test_hsm.transition_to( s2, event );
    }

    static auto s( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s-EXIT;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    test_hsm.m_stream << "s-INIT;";
                    return test_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::E:
                    test_hsm.m_stream << "s-E;";
                    return test_hsm.transition_to( s11, event );
                case Event::I:
                    if ( test_hsm.m_foo ) {
                        test_hsm.m_foo = false;
                        test_hsm.m_stream << "s-I;";
                        return test_hsm.event_handled( event );
                    } // if
                    break;
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( top, event );
    }

    static auto s1( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s1-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s1-EXIT;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    test_hsm.m_stream << "s1-INIT;";
                    return test_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    test_hsm.m_stream << "s1-A;";
                    return test_hsm.transition_to( s1, event );
                case Event::B:
                    test_hsm.m_stream << "s1-B;";
                    return test_hsm.transition_to( s11, event );
                case Event::C:
                    test_hsm.m_stream << "s1-C;";
                    return test_hsm.transition_to( s2, event );
                case Event::D:
                    if ( not test_hsm.m_foo ) {
                        test_hsm.m_foo = true;
                        test_hsm.m_stream << "s1-D;";
                        return test_hsm.transition_to( s, event );
                    } // if
                    break;
                case Event::F:
                    test_hsm.m_stream << "s1-F;";
                    return test_hsm.transition_to( s211, event );
                case Event::I:
                    test_hsm.m_stream << "s1-I;";
                    return test_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( s, event );
    }

    static auto s11( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s11-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s11-EXIT;";
                    return test_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::D:
                    if ( test_hsm.m_foo ) {
                        test_hsm.m_foo = false;
                        test_hsm.m_stream << "s11-D;";
                        return test_hsm.transition_to( s1, event );
                    } // if
                    break;
                case Event::G:
                    test_hsm.m_stream << "s11-G;";
                    return test_hsm.transition_to( s211, event );
                case Event::H:
                    test_hsm.m_stream << "s11-H;";
                    return test_hsm.transition_to( s, event );
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( s1, event );
    }

    static auto s2( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s2-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s2-EXIT;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    test_hsm.m_stream << "s2-INIT;";
                    return test_hsm.transition_to( s211, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::C:
                    test_hsm.m_stream << "s2-C;";
                    return test_hsm.transition_to( s1, event );
                case Event::F:
                    test_hsm.m_stream << "s2-F;";
                    return test_hsm.transition_to( s11, event );
                case Event::I:
                    if ( not test_hsm.m_foo ) {
                        test_hsm.m_foo = true;
                        test_hsm.m_stream << "s2-I;";
                        return test_hsm.event_handled( event );
                    } // if
                    break;
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( s, event );
    }

    static auto s21( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s21-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s21-EXIT;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    test_hsm.m_stream << "s21-INIT;";
                    return test_hsm.transition_to( s211, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    test_hsm.m_stream << "s21-A;";
                    return test_hsm.transition_to( s21, event );
                case Event::B:
                    test_hsm.m_stream << "s21-B;";
                    return test_hsm.transition_to( s211, event );
                case Event::G:
                    test_hsm.m_stream << "s21-G;";
                    return test_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( s2, event );
    }

    static auto s211( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & test_hsm = static_cast<Test_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    test_hsm.m_stream << "s211-ENTRY;";
                    return test_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    test_hsm.m_stream << "s211-EXIT;";
                    return test_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::D:
                    test_hsm.m_stream << "s211-D;";
                    return test_hsm.transition_to( s21, event );
                case Event::H:
                    test_hsm.m_stream << "s211-H;";
                    return test_hsm.transition_to( s, event );
                default: break;
            } // switch
        }     // if

        return test_hsm.defer_event_handling_to( s21, event );
    }
};

Test_HSM::Event_Category const Test_HSM::Event_Category::INSTANCE{};

Simple_Event const Test_HSM::TOPMOST_INITIAL_TRANSITION_TRIGGER{
    Event_Category::instance(),
    to_underlying( Event::TOPMOST_INITIAL_TRANSITION_TRIGGER )
};

Simple_Event const Test_HSM::A{ Event_Category::instance(), to_underlying( Event::A ) };
Simple_Event const Test_HSM::B{ Event_Category::instance(), to_underlying( Event::B ) };
Simple_Event const Test_HSM::C{ Event_Category::instance(), to_underlying( Event::C ) };
Simple_Event const Test_HSM::D{ Event_Category::instance(), to_underlying( Event::D ) };
Simple_Event const Test_HSM::E{ Event_Category::instance(), to_underlying( Event::E ) };
Simple_Event const Test_HSM::F{ Event_Category::instance(), to_underlying( Event::F ) };
Simple_Event const Test_HSM::G{ Event_Category::instance(), to_underlying( Event::G ) };
Simple_Event const Test_HSM::H{ Event_Category::instance(), to_underlying( Event::H ) };
Simple_Event const Test_HSM::I{ Event_Category::instance(), to_underlying( Event::I ) };

} // namespace

/**
 * \brief Verify picolibrary::HSM works properly.
 */
TEST( hsm, worksProperly )
{
    auto stream = std::ostringstream{};

    auto test_hsm = Test_HSM{ stream };

    test_hsm.execute_topmost_initial_transition();
    stream << '\n';

    Simple_Event const event_sequence[]{
        // clang-format off

        Test_HSM::G,
        Test_HSM::I,
        Test_HSM::A,
        Test_HSM::D,
        Test_HSM::D,
        Test_HSM::C,
        Test_HSM::E,
        Test_HSM::E,
        Test_HSM::G,
        Test_HSM::I,
        Test_HSM::I,

        // clang-format on
    };

    for ( auto const & event : event_sequence ) {
        test_hsm.dispatch( event );
        stream << '\n';
    } // for

    EXPECT_EQ(
        stream.str(),
        "top-INIT;s-ENTRY;s2-ENTRY;s2-INIT;s21-ENTRY;s211-ENTRY;\n"
        "s21-G;s211-EXIT;s21-EXIT;s2-EXIT;s1-ENTRY;s11-ENTRY;\n"
        "s1-I;\n"
        "s1-A;s11-EXIT;s1-EXIT;s1-ENTRY;s1-INIT;s11-ENTRY;\n"
        "s1-D;s11-EXIT;s1-EXIT;s-INIT;s1-ENTRY;s11-ENTRY;\n"
        "s11-D;s11-EXIT;s1-INIT;s11-ENTRY;\n"
        "s1-C;s11-EXIT;s1-EXIT;s2-ENTRY;s2-INIT;s21-ENTRY;s211-ENTRY;\n"
        "s-E;s211-EXIT;s21-EXIT;s2-EXIT;s1-ENTRY;s11-ENTRY;\n"
        "s-E;s11-EXIT;s1-EXIT;s1-ENTRY;s11-ENTRY;\n"
        "s11-G;s11-EXIT;s1-EXIT;s2-ENTRY;s21-ENTRY;s211-ENTRY;\n"
        "s2-I;\n"
        "s-I;\n" );
}

/**
 * \brief Execute the picolibrary::HSM automated tests.
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
