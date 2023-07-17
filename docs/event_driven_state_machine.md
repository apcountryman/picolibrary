# Event Driven State Machine Facilities

## Table of Contents
1. [Event Identification](#event-identification)
    1. [Defining Events](#defining-events)
1. [Event Storage](#event-storage)
1. [Non-Hierarchical State Machines](#non-hierarchical-state-machines)
1. [HSMs](#hsms)

## Event Identification
Event identification facilities are defined in the
[`include/picolibrary/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/event.h)/[`source/picolibrary/event.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/event.cc)
header/source file pair.

The `::picolibrary::Event` class is used to identify events.
An event's type can be determined using the combination of the address of the event's
event category (`::picolibrary::Event_Category`) and the event's event ID
(`::picolibrary::Event_ID`).
- To get a reference to an event's event category, use the
  `::picolibrary::Event::category()` member function.
- To get an event's event ID, use the `::picolibrary::Event::id()` member function.

Additional information can be added to an event type by creating a class that derives from
`::picolibrary::Event`.
The additional information can be made printable by overriding the
`::picolibrary::Event::print_details()` member function.
The `::picolibrary::Event::print_details()` member function can only be overridden if the
`PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION` project configuration option is
`OFF`.

`::picolibrary::Event` automated tests are defined in the
[`test/automated/picolibrary/event/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/event/main.cc)
source file.

A `std::ostream` insertion operator is defined for `::picolibrary::Event` if the
`PICOLIBRARY_ENABLE_AUTOMATED_TESTING` project configuration option is `ON`.
The insertion operator is defined in the
[`include/picolibrary/testing/automated/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/event.h)/[`source/picolibrary/testing/automated/event.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/event.cc)
header/source file pair.

A mock event class (`::picolibrary::Testing::Automated::Mock_Event`) is defined in the
[`include/picolibrary/testing/automated/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/event.h)/[`source/picolibrary/testing/automated/event.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/event.cc)
header/source file pair.

`::picolibrary::Output_Formatter` is specialized for `::picolibrary::Event`.
The `::picolibrary::Output_Formatter<::picolibrary::Event>` specialization does not
support user formatting configuration.
`::picolibrary::Output_Formatter<::picolibrary::Event>` automated tests are defined in the
[`test/automated/picolibrary/event/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/event/main.cc)
source file.

The `::picolibrary::Event_Category` class is used to get human readable event information.
- To get an event category's name, use the `::picolibrary::Event_Category::name()` member
  function.
- To get an event ID's description, use the
  `::picolibrary::Event_Category::event_description()` member function.

If the `PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION` project configuration
option is `ON`, these functions will return empty strings.

A mock event category class (`::picolibrary::Testing::Automated::Mock_Event_Category`) is
defined in the
[`include/picolibrary/testing/automated/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/testing/automated/event.h)/[`source/picolibrary/testing/automated/event.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/testing/automated/event.cc)
header/source file pair.

The `::picolibrary::Simple_Event` class is used to easily define simple events (events
that contain no information beyond their type).
Implicit conversion from an enum class to a `::picolibrary::Simple_Event` is enabled if
`::picolibrary::is_simple_event_enum` has been appropriately specialized for the enum
class.

### Defining Events
To define an event set, do the following:
1. Define an enum class whose enumerators identify the types of events in the set.
   The underlying type for the enum class should be `::picolibrary::Event_ID`.
2. Create an event category class for the event set.
   `::picolibrary::Event_Category` must be a public base of this class.
   This class should be a singleton.
3. Create an event class for each event type in the set that carries information beyond
   its event type.
   `::picolibrary::Event` must be a public base of these classes.
   The additional information is made printable by overriding the
   `::picolibrary::Event::print_details()` member function.
4. If all events in the event set are simple events (optional):
    1. Define a `make_simple_event()` function in the same namespace as the enum class
       that takes the enum class and returns a `::picolibrary::Simple_Event`.
    2. Register the enum class as a simple event enum by specializing
       `::picolibrary::is_simple_event_enum` for the enum class.

## Event Storage
The `::picolibrary::Event_Storage` class is used to store multiple types of events.
The `::picolibrary::Event_Storage` class is defined in the
[`include/picolibrary/event.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/event.h)/[`source/picolibrary/event.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/event.cc)
header/source file pair.
- To access a stored event, use the `::picolibrary::Event_Storage::event()` member
  function.

## Non-Hierarchical State Machines
The `::picolibrary::State_Machine` non-hierarchical state machine with support for state
entry and exit actions base class is defined in the
[`include/picolibrary/state_machine.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/state_machine.h)/[`source/picolibrary/state_machine.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/state_machine.cc)
header/source file pair.

The `::picolibrary::State_Machine::State_Event_Handler` member type alias defines the
state event handler function signature.
See
[`include/picolibrary/state_machine.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/state_machine.h)
for more information about state event handlers.

`::picolibrary::State_Machine` supports the following operations:
- To execute a state machine's initial transition from a derived class, use the
  `::picolibrary::State_Machine::execute_initial_transition()` member function.
- To dispatch an event to the event handler for a state machine's currently active state
  from a derived class, use the `::picolibrary::State_Machine::dispatch()` member
  function.
- To get the state event handler for a state machine's currently active state from a
  derived class, use the `::picolibrary::State_Machine::current_state()` member function.
- To check if a state event handler is the state event handler for a state machine's
  currently active state from a derived class, use the
  `::picolibrary::State_Machine::is_in()` member function.

`::picolibrary::State_Machine` automated tests are defined in the
[`test/automated/picolibrary/state_machine/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/state_machine/main.cc)
source file.

The following example assumes that the
`PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION` project configuration option is
`ON`.
```c++
#include <cstdlib>
#include <iostream>

#include "picolibrary/error.h"
#include "picolibrary/event.h"
#include "picolibrary/fatal_error.h"
#include "picolibrary/rom.h"
#include "picolibrary/state_machine.h"
#include "picolibrary/utility.h"

namespace picolibrary {

void trap_fatal_error( ROM::String file, int line, Error_Code const & error ) noexcept
{
    std::cerr << "fatal error: " << file << ':' << line << ": " << error.category().name()
              << "::" << error.description() << '\n';

    std::abort();
}

} // namespace picolibrary

namespace {

class Example_State_Machine : public ::picolibrary::State_Machine {
  public:
    enum class Event : ::picolibrary::Event_ID {
        INITIAL_TRANSITION_TRIGGER,
        A,
        B,
        C,
        TERMINATE,
    };

    static ::picolibrary::Simple_Event const A;
    static ::picolibrary::Simple_Event const B;
    static ::picolibrary::Simple_Event const C;
    static ::picolibrary::Simple_Event const TERMINATE;

    constexpr Example_State_Machine() noexcept :
        ::picolibrary::State_Machine{ initial_pseudostate }
    {
    }

    Example_State_Machine( Example_State_Machine && ) = delete;

    Example_State_Machine( Example_State_Machine const & ) = delete;

    ~Example_State_Machine() noexcept = default;

    auto operator=( Example_State_Machine && ) = delete;

    auto operator=( Example_State_Machine const & ) = delete;

    void execute_initial_transition() noexcept
    {
        ::picolibrary::State_Machine::execute_initial_transition( INITIAL_TRANSITION_TRIGGER );
    }

    using ::picolibrary::State_Machine::dispatch;

  private:
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
    };

    static ::picolibrary::Simple_Event const INITIAL_TRANSITION_TRIGGER;

    static auto initial_pseudostate( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & example_state_machine = static_cast<Example_State_Machine &>( state_machine );

        std::cout << "start-INIT;";

        return example_state_machine.transition_to( s1, event );
    }

    static auto s1( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & example_state_machine = static_cast<Example_State_Machine &>( state_machine );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s1-ENTRY;";
                    return example_state_machine.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s1-EXIT;";
                    return example_state_machine.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    std::cout << "s1-A;";
                    return example_state_machine.event_handled( event );
                case Event::B:
                    std::cout << "s1-B;";
                    return example_state_machine.transition_to( s1, event );
                case Event::C:
                    std::cout << "s1-C;";
                    return example_state_machine.transition_to( s2, event );
                case Event::TERMINATE: std::cout << "s1-TERMINATE;\n"; std::exit( 0 );
                default: break;
            } // switch
        }     // if

        return example_state_machine.event_handled( event );
    }

    static auto s2( State_Machine & state_machine, ::picolibrary::Event const & event ) noexcept
        -> Event_Handling_Result
    {
        auto & example_state_machine = static_cast<Example_State_Machine &>( state_machine );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s2-ENTRY;";
                    return example_state_machine.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s2-EXIT;";
                    return example_state_machine.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    std::cout << "s2-A;";
                    return example_state_machine.event_handled( event );
                case Event::B:
                    std::cout << "s2-B;";
                    return example_state_machine.transition_to( s2, event );
                case Event::C:
                    std::cout << "s2-C;";
                    return example_state_machine.transition_to( s1, event );
                case Event::TERMINATE: std::cout << "s2-TERMINATE;\n"; std::exit( 0 );
                default: break;
            } // switch
        }     // if

        return example_state_machine.event_handled( event );
    }
};

Example_State_Machine::Event_Category const Example_State_Machine::Event_Category::INSTANCE{};

::picolibrary::Simple_Event const Example_State_Machine::INITIAL_TRANSITION_TRIGGER{ Event_Category::instance(), ::picolibrary::to_underlying( Event::INITIAL_TRANSITION_TRIGGER ) };
::picolibrary::Simple_Event const Example_State_Machine::A{ Event_Category::instance(), ::picolibrary::to_underlying( Event::A ) };
::picolibrary::Simple_Event const Example_State_Machine::B{ Event_Category::instance(), ::picolibrary::to_underlying( Event::B ) };
::picolibrary::Simple_Event const Example_State_Machine::C{ Event_Category::instance(), ::picolibrary::to_underlying( Event::C ) };
::picolibrary::Simple_Event const Example_State_Machine::TERMINATE{ Event_Category::instance(), ::picolibrary::to_underlying( Event::TERMINATE ) };

} // namespace

int main()
{
    auto example_state_machine = ::Example_State_Machine{};

    example_state_machine.execute_initial_transition();
    std::cout << '\n';

    ::picolibrary::Simple_Event const event_sequence[]{
        ::Example_State_Machine::A,
        ::Example_State_Machine::B,
        ::Example_State_Machine::C,
        ::Example_State_Machine::A,
        ::Example_State_Machine::B,
        ::Example_State_Machine::C,
        ::Example_State_Machine::TERMINATE,
    };

    for ( auto const & event : event_sequence ) {
        example_state_machine.dispatch( event );
        std::cout << '\n';
    } // for
}
```
```
start-INIT;s1-ENTRY;
s1-A;
s1-B;s1-EXIT;s1-ENTRY;
s1-C;s1-EXIT;s2-ENTRY;
s2-A;
s2-B;s2-EXIT;s2-ENTRY;
s2-C;s2-EXIT;s1-ENTRY;
s1-TERMINATE;
```

## HSMs
The `::picolibrary::HSM` HSM base class is defined in the
[`include/picolibrary/hsm.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/hsm.h)/[`source/picolibrary/hsm.cc`](https://github.com/apcountryman/picolibrary/blob/main/source/picolibrary/hsm.cc)
header/source file pair.

The maximum state depth (not including the implicit "top" superstate) is 8.

The `::picolibrary::HSM::State_Event_Handler` member type alias defines the state event
handler function signature.
See
[`include/picolibrary/hsm.h`](https://github.com/apcountryman/picolibrary/blob/main/include/picolibrary/hsm.h)
for more information about state event handlers.

`::picolibrary::HSM` supports the following operations:
- To execute an HSM's topmost initial transition from a derived class, use the
  `::picolibrary::HSM::execute_topmost_initial_transition()` member function.
- To dispatch an event to the event handler for an HSM's currently active state from a
  derived class, use the `::picolibrary::HSM::dispatch()` member function.
- To get the state event handler for an HSM's currently active state from a derived class,
  use the `::picolibrary::HSM::current_state()` member function.
- To check if a state event handler is the state event handler for an HSM's currently
  active state or one of its superstates from a derived class, use the
  `::picolibrary::HSM::is_in()` member function.

`::picolibrary::HSM` automated tests are defined in the
[`test/automated/picolibrary/hsm/main.cc`](https://github.com/apcountryman/picolibrary/blob/main/test/automated/picolibrary/hsm/main.cc)
source file.

The following example is based on "Practical UML Statecharts in C/C++, Second Edition:
Event-Driven Programming for Embedded Systems" Figure 2.11.
Note that there is an error in line "(2)" of Figure 2.12 ("s1-INIT;" should not be
present).
The example assumes that the `PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION`
project configuration option is `OFF`.
```c++
#include <cstdlib>
#include <iostream>

#include "picolibrary/error.h"
#include "picolibrary/event.h"
#include "picolibrary/fatal_error.h"
#include "picolibrary/hsm.h"
#include "picolibrary/rom.h"
#include "picolibrary/utility.h"

namespace picolibrary {

void trap_fatal_error( ROM::String file, int line, Error_Code const & error ) noexcept
{
    std::cerr << "fatal error: " << file << ':' << line << ": " << error.category().name()
              << "::" << error.description() << '\n';

    std::abort();
}

} // namespace picolibrary

namespace {

class Example_HSM : public ::picolibrary::HSM {
  public:
    enum class Event : ::picolibrary::Event_ID {
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
        TERMINATE,
    };

    static ::picolibrary::Simple_Event const A;
    static ::picolibrary::Simple_Event const B;
    static ::picolibrary::Simple_Event const C;
    static ::picolibrary::Simple_Event const D;
    static ::picolibrary::Simple_Event const E;
    static ::picolibrary::Simple_Event const F;
    static ::picolibrary::Simple_Event const G;
    static ::picolibrary::Simple_Event const H;
    static ::picolibrary::Simple_Event const I;
    static ::picolibrary::Simple_Event const TERMINATE;

    constexpr Example_HSM() noexcept : ::picolibrary::HSM{ initial_pseudostate }
    {
    }

    Example_HSM( Example_HSM && ) = delete;

    Example_HSM( Example_HSM const & ) = delete;

    ~Example_HSM() noexcept = default;

    auto operator=( Example_HSM && ) = delete;

    auto operator=( Example_HSM const & ) = delete;

    void execute_topmost_initial_transition() noexcept
    {
        ::picolibrary::HSM::execute_topmost_initial_transition( TOPMOST_INITIAL_TRANSITION_TRIGGER );
    }

    using ::picolibrary::HSM::dispatch;

  private:
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
    };

    static ::picolibrary::Simple_Event const TOPMOST_INITIAL_TRANSITION_TRIGGER;

    bool m_foo{};

    static auto initial_pseudostate( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        example_hsm.m_foo = false;

        std::cout << "top-INIT;";

        return example_hsm.transition_to( s2, event );
    }

    static auto s( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s-EXIT;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    std::cout << "s-INIT;";
                    return example_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::E:
                    std::cout << "s-E;";
                    return example_hsm.transition_to( s11, event );
                case Event::I:
                    if ( example_hsm.m_foo ) {
                        example_hsm.m_foo = false;
                        std::cout << "s-I;";
                        return example_hsm.event_handled( event );
                    } // if
                    break;
                case Event::TERMINATE: std::cout << "s-TERMINATE;\n"; std::exit( 0 );
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( top, event );
    }

    static auto s1( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s1-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s1-EXIT;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    std::cout << "s1-INIT;";
                    return example_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    std::cout << "s1-A;";
                    return example_hsm.transition_to( s1, event );
                case Event::B:
                    std::cout << "s1-B;";
                    return example_hsm.transition_to( s11, event );
                case Event::C:
                    std::cout << "s1-C;";
                    return example_hsm.transition_to( s2, event );
                case Event::D:
                    if ( not example_hsm.m_foo ) {
                        example_hsm.m_foo = true;
                        std::cout << "s1-D;";
                        return example_hsm.transition_to( s, event );
                    } // if
                    break;
                case Event::F:
                    std::cout << "s1-F;";
                    return example_hsm.transition_to( s211, event );
                case Event::I:
                    std::cout << "s1-I;";
                    return example_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( s, event );
    }

    static auto s11( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s11-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s11-EXIT;";
                    return example_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::D:
                    if ( example_hsm.m_foo ) {
                        example_hsm.m_foo = false;
                        std::cout << "s11-D;";
                        return example_hsm.transition_to( s1, event );
                    } // if
                    break;
                case Event::G:
                    std::cout << "s11-G;";
                    return example_hsm.transition_to( s211, event );
                case Event::H:
                    std::cout << "s11-H;";
                    return example_hsm.transition_to( s, event );
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( s1, event );
    }

    static auto s2( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s2-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s2-EXIT;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    std::cout << "s2-INIT;";
                    return example_hsm.transition_to( s211, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::C:
                    std::cout << "s2-C;";
                    return example_hsm.transition_to( s1, event );
                case Event::F:
                    std::cout << "s2-F;";
                    return example_hsm.transition_to( s11, event );
                case Event::I:
                    if ( not example_hsm.m_foo ) {
                        example_hsm.m_foo = true;
                        std::cout << "s2-I;";
                        return example_hsm.event_handled( event );
                    } // if
                    break;
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( s, event );
    }

    static auto s21( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s21-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s21-EXIT;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    std::cout << "s21-INIT;";
                    return example_hsm.transition_to( s211, event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::A:
                    std::cout << "s21-A;";
                    return example_hsm.transition_to( s21, event );
                case Event::B:
                    std::cout << "s21-B;";
                    return example_hsm.transition_to( s211, event );
                case Event::G:
                    std::cout << "s21-G;";
                    return example_hsm.transition_to( s11, event );
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( s2, event );
    }

    static auto s211( HSM & hsm, ::picolibrary::Event const & event ) noexcept -> Event_Handling_Result
    {
        auto & example_hsm = static_cast<Example_HSM &>( hsm );

        if ( &event.category() == &Pseudo_Event_Category::instance() ) {
            switch ( static_cast<Pseudo_Event>( event.id() ) ) {
                case Pseudo_Event::ENTRY:
                    std::cout << "s211-ENTRY;";
                    return example_hsm.event_handled( event );
                case Pseudo_Event::EXIT:
                    std::cout << "s211-EXIT;";
                    return example_hsm.event_handled( event );
                default: break;
            } // switch
        }     // if

        if ( &event.category() == &Event_Category::instance() ) {
            switch ( static_cast<Event>( event.id() ) ) {
                case Event::D:
                    std::cout << "s211-D;";
                    return example_hsm.transition_to( s21, event );
                case Event::H:
                    std::cout << "s211-H;";
                    return example_hsm.transition_to( s, event );
                default: break;
            } // switch
        }     // if

        return example_hsm.defer_event_handling_to( s21, event );
    }
};

Example_HSM::Event_Category const Example_HSM::Event_Category::INSTANCE{};

::picolibrary::Simple_Event const Example_HSM::TOPMOST_INITIAL_TRANSITION_TRIGGER{ Event_Category::instance(), ::picolibrary::to_underlying( Event::TOPMOST_INITIAL_TRANSITION_TRIGGER ) };
::picolibrary::Simple_Event const Example_HSM::A{ Event_Category::instance(), ::picolibrary::to_underlying( Event::A ) };
::picolibrary::Simple_Event const Example_HSM::B{ Event_Category::instance(), ::picolibrary::to_underlying( Event::B ) };
::picolibrary::Simple_Event const Example_HSM::C{ Event_Category::instance(), ::picolibrary::to_underlying( Event::C ) };
::picolibrary::Simple_Event const Example_HSM::D{ Event_Category::instance(), ::picolibrary::to_underlying( Event::D ) };
::picolibrary::Simple_Event const Example_HSM::E{ Event_Category::instance(), ::picolibrary::to_underlying( Event::E ) };
::picolibrary::Simple_Event const Example_HSM::F{ Event_Category::instance(), ::picolibrary::to_underlying( Event::F ) };
::picolibrary::Simple_Event const Example_HSM::G{ Event_Category::instance(), ::picolibrary::to_underlying( Event::G ) };
::picolibrary::Simple_Event const Example_HSM::H{ Event_Category::instance(), ::picolibrary::to_underlying( Event::H ) };
::picolibrary::Simple_Event const Example_HSM::I{ Event_Category::instance(), ::picolibrary::to_underlying( Event::I ) };
::picolibrary::Simple_Event const Example_HSM::TERMINATE{ Event_Category::instance(), ::picolibrary::to_underlying( Event::TERMINATE ) };

} // namespace

int main()
{
    auto example_hsm = ::Example_HSM{};

    example_hsm.execute_topmost_initial_transition();
    std::cout << '\n';

    ::picolibrary::Simple_Event const event_sequence[]{
        ::Example_HSM::G,
        ::Example_HSM::I,
        ::Example_HSM::A,
        ::Example_HSM::D,
        ::Example_HSM::D,
        ::Example_HSM::C,
        ::Example_HSM::E,
        ::Example_HSM::E,
        ::Example_HSM::G,
        ::Example_HSM::I,
        ::Example_HSM::I,
        ::Example_HSM::TERMINATE,
    };

    for ( auto const & event : event_sequence ) {
        example_hsm.dispatch( event );
        std::cout << '\n';
    } // for
}
```
```
top-INIT;s-ENTRY;s2-ENTRY;s2-INIT;s21-ENTRY;s211-ENTRY;
s21-G;s211-EXIT;s21-EXIT;s2-EXIT;s1-ENTRY;s11-ENTRY;
s1-I;
s1-A;s11-EXIT;s1-EXIT;s1-ENTRY;s1-INIT;s11-ENTRY;
s1-D;s11-EXIT;s1-EXIT;s-INIT;s1-ENTRY;s11-ENTRY;
s11-D;s11-EXIT;s1-INIT;s11-ENTRY;
s1-C;s11-EXIT;s1-EXIT;s2-ENTRY;s2-INIT;s21-ENTRY;s211-ENTRY;
s-E;s211-EXIT;s21-EXIT;s2-EXIT;s1-ENTRY;s11-ENTRY;
s-E;s11-EXIT;s1-EXIT;s1-ENTRY;s11-ENTRY;
s11-G;s11-EXIT;s1-EXIT;s2-ENTRY;s21-ENTRY;s211-ENTRY;
s2-I;
s-I;
s-TERMINATE;
```
