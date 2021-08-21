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
 * \brief picolibrary::HSM interface.
 */

#ifndef PICOLIBRARY_HSM_H
#define PICOLIBRARY_HSM_H

#include <cstddef>
#include <type_traits>

#include "picolibrary/event.h"
#include "picolibrary/fatal_error.h"
#include "picolibrary/fixed_size_array.h"

namespace picolibrary {

/**
 * \brief Hierarchical State Machine (HSM).
 */
class HSM {
  public:
    /**
     * \brief Destructor.
     */
    ~HSM() noexcept = default;

  protected:
    /**
     * \brief Pseudo-event.
     */
    enum class Pseudo_Event : Event_ID {
        DISCOVERY, ///< Discovering state hierarchy, defer event handling to superstate.
        ENTRY,     ///< State entered, execute entry actions.
        EXIT,      ///< State exited, execute exit actions.
        NESTED_INITIAL_TRANSITION, ///< State entered, entry actions executed, execute nested initial transition and associated actions.
    };

    /**
     * \brief Pseudo-event category.
     */
    class Pseudo_Event_Category final : public Event_Category {
      public:
        /**
         * \brief Get a reference to the pseudo-event category instance.
         *
         * \return A reference to the pseudo-event category instance.
         */
        static constexpr auto const & instance() noexcept
        {
            return INSTANCE;
        }

        Pseudo_Event_Category( Pseudo_Event_Category && ) = delete;

        Pseudo_Event_Category( Pseudo_Event_Category const & ) = delete;

        auto operator=( Pseudo_Event_Category && ) = delete;

        auto operator=( Pseudo_Event_Category const & ) = delete;

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        /**
         * \brief Get the name of the event category.
         *
         * \return The name of the event category.
         */
        virtual auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::HSM::Pseudo_Event";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        /**
         * \brief Get an event ID's description.
         *
         * \param[in] id The event ID whose description is to be got.
         *
         * \return The event ID's description.
         */
        virtual auto event_description( Event_ID id ) const noexcept -> char const * override final
        {
            switch ( static_cast<Pseudo_Event>( id ) ) {
                case Pseudo_Event::DISCOVERY: return "DISCOVERY";
                case Pseudo_Event::ENTRY: return "ENTRY";
                case Pseudo_Event::EXIT: return "EXIT";
                case Pseudo_Event::NESTED_INITIAL_TRANSITION:
                    return "NESTED_INITIAL_TRANSITION";
            } // switch

            return "UNKNOWN";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

      private:
        /**
         * \brief The pseudo-event category instance.
         */
        static Pseudo_Event_Category const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Pseudo_Event_Category() noexcept = default;

        /**
         * \brief Destructor.
         */
        ~Pseudo_Event_Category() noexcept = default;
    };

    /**
     * \brief Event handling result.
     */
    enum class Event_Handling_Result : std::uint_fast8_t {
        EVENT_IGNORED,                         ///< Event ignored.
        EVENT_HANDLED,                         ///< Event handled.
        EVENT_HANDLING_DEFERRED_TO_SUPERSTATE, ///< Event handling deferred to superstate.
        STATE_TRANSITION_TRIGGERED,            ///< State transition triggered.
    };

    /**
     * \brief State event handler.
     *
     * Entry actions:
     * If a state has entry actions, the state's event handler must execute the state's
     * entry actions when it is passed the entry pseudo-event. Once the state's entry
     * actions have been executed, the state's event handler must report that the entry
     * pseudo-event has been handled. The entry pseudo-event can be identified and acted
     * upon as follows:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance()) {
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::ENTRY ) {
     *         // execute entry actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have entry actions, the state's event handler must defer
     * handling of the entry pseudo-event to the state event handler for the state's
     * superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( foo );
     * \endcode
     *
     * Exit actions:
     * If a state has exit actions, the state's event handler must execute the state's
     * exit actions when it is passed the exit pseudo-event. Once the state's exit actions
     * have been executed, the state's event handler must report that the exit
     * pseudo-event has been handled. The exit pseudo-event can be identified and acted
     * upon as follows:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance()) {
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::EXIT ) {
     *         // execute exit actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have exit actions, the state's event handler must defer
     * handling of the exit pseudo-event to the state event handler for the state's
     * superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( foo, event );
     * \endcode
     *
     * Nested Initial transition:
     * If a state has a nested initial transition, the state's event handler must execute
     * the nested initial transition (and any associated actions) when it is passed the
     * nested initial transition pseudo-event. The nested initial transition pseudo-event
     * can be identified and acted upon as follows:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance()) {
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::NESTED_INITIAL_TRANSITION ) {
     *         // execute any actions associated with the nested initial transition
     *
     *         return hsm.transition_to( target_state );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have a nested initial transition, the state's event handler
     * must defer handling of the nested initial transition pseudo-event as follows:
     * \code
     * return hsm.defer_event_handling_to( target_state, event );
     * \endcode
     *
     * Event handling:
     * If a state handles an event, the state's event handler must report report that the
     * event has been handled as follows:
     * \code
     * return hsm.event_handled( event );
     * \endcode
     *
     * Event handling deferral:
     * If the state does not explicitly handle an event, the state's event handler must
     * defer handling of the event to state event handler for the state's superstate as
     * follows:
     * \code
     * return hsm.defer_event_handling_to( superstate, event );
     * \endcode
     *
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the event to the implicit "top"
     * superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Summary:
     * \code
     * // entry actions, omit block if the state has no entry actions
     * if ( &event.category() == &Pseudo_Event_Category::instance()) {
     *     // entry actions, omit block if the state has no entry actions
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::ENTRY ) {
     *         // execute entry actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     *
     *     // exit actions, omit block if the state has no exit actions
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::EXIT ) {
     *         // execute exit actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     *
     *     // nested initial transition, omit block if the state does not have a nested
     *     // initial transition
     *     if ( static_cast<Pseudo_Event>( event.id()) == Pseudo_Event::NESTED_INITIAL_TRANSITION ) {
     *         // execute any actions associated with the nested initial transition
     *
     *         return hsm.transition_to( target_state );
     *     } // if
     * } // if
     *
     * return hsm.defer_event_handling_to( superstate, event );
     * \endcode
     */
    using State_Event_Handler = auto( HSM & hsm, Event const & event ) noexcept -> Event_Handling_Result;

    /**
     * \brief State event handler pointer.
     */
    using State_Event_Handler_Pointer = std::add_pointer_t<State_Event_Handler>;

    /**
     * \brief State event handler reference.
     */
    using State_Event_Handler_Reference = std::add_lvalue_reference_t<State_Event_Handler>;

    /**
     * \brief Implicit "top" superstate (ultimate root of the state hierarchy) event
     *        handler.
     *
     * \warning This state event handler must never be the target of a state transition.
     *
     * \param[in] hsm The HSM.
     * \param[in] event The event that will be ignored.
     *
     * \return Event ignored event handling result.
     */
    static constexpr auto top( HSM & hsm, Event const & event ) noexcept -> Event_Handling_Result
    {
        return hsm.event_ignored( event );
    }

    /**
     * \brief Constructor.
     */
    constexpr HSM() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] initial_pseudostate The state event handler for the initial pseudostate.
     *            The state event handler for the initial pseudostate must trigger the
     *            initial state transition by calling picolibrary::HSM::transition_to().
     */
    constexpr HSM( State_Event_Handler_Reference initial_pseudostate ) noexcept :
        m_current_state{ &initial_pseudostate }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr HSM( HSM && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr HSM( HSM const & original ) noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( HSM && expression ) noexcept -> HSM & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( HSM const & expression ) noexcept -> HSM & = default;

    /**
     * \brief Report that an event has been handled.
     *
     * \param[in] event The event that has been handled.
     *
     * \return Event handled event handling result.
     */
    constexpr auto event_handled( Event const & event ) const noexcept -> Event_Handling_Result
    {
        static_cast<void>( event );

        return Event_Handling_Result::EVENT_HANDLED;
    }

    /**
     * \brief Defer handling of an event to the superstate.
     *
     * \param[in] superstate The state event handler for the superstate.
     * \param[in] event The event whose handling is being deferred.
     *
     * \return Event handling deferred to superstate event handling result.
     */
    constexpr auto defer_event_handling_to( State_Event_Handler_Reference superstate, Event const & event ) noexcept
    {
        static_cast<void>( event );

        m_superstate = superstate;

        return Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE;
    }

    /**
     * \brief Trigger a state transition.
     *
     * \param[in] target_state The state event handler for the target of the state
     *            transition.
     * \param[in] trigger The event that triggered the state transition.
     *
     * \return State transition triggered event handling result.
     */
    constexpr auto transition_to( State_Event_Handler_Reference target_state, Event const & trigger ) noexcept
    {
        static_cast<void>( trigger );

        m_target_state = target_state;

        return Event_Handling_Result::STATE_TRANSITION_TRIGGERED;
    }

    /**
     * \brief Execute the HSM's topmost initial transition and any resulting nested
     *        initial transitions.
     *
     * \param[in] event The event to pass to the initial pseudostate's event handler to
     *            trigger the topmost initial transition.
     */
    void execute_topmost_initial_transition( Event const & event ) noexcept
    {
        // #lizard forgives the length

        if ( ( *m_current_state )( *this, event ) != Event_Handling_Result::STATE_TRANSITION_TRIGGERED ) {
            trap_fatal_error();
        } // if

        auto main_source_state = top;
        auto main_target_state = m_target_state;
        for ( ;; ) {
            m_current_state = main_target_state;

            Path path;
            path[ 0 ] = main_target_state;

            if ( ( *main_target_state )( *this, Discovery::instance() )
                 != Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE ) {
                trap_fatal_error();
            } // if
            auto i = std::size_t{ 1 };
            for ( ; m_superstate != main_source_state; ++i ) {
                if ( i >= path.size() ) {
                    trap_fatal_error();
                } // if

                path[ i ] = m_superstate;

                if ( ( *m_superstate )( *this, Discovery::instance() )
                     != Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE ) {
                    trap_fatal_error();
                } // if
            }     // for

            while ( --i ) {
                switch ( ( *path[ i ] )( *this, Entry::instance() ) ) {
                    case Event_Handling_Result::EVENT_HANDLED: break;
                    case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE:
                        break;
                    default: trap_fatal_error();
                } // switch
            }     // while

            switch ( ( *path[ i ] )( *this, Nested_Initial_Transition::instance() ) ) {
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                    main_source_state = main_target_state;
                    main_target_state = m_target_state;
                default: trap_fatal_error();
            } // switch
        }     // for
    }

    /**
     * \brief Get the state event handler for the current state.
     *
     * \return The state event handler for the current state.
     */
    constexpr auto current_state() const noexcept
    {
        return m_current_state;
    }

    /**
     * \brief Check if a state event handler is the state event handler for the current
     *        state or one of its superstates.
     *
     * \param[in] state The state event handler to check if it is the state event handler
     *            for the current state or one of its superstates.
     *
     * \return true if state is the state event handler for the current state or one of
     *         its superstates.
     * \return false if state is neither the state event handler for the current state nor
     *         one of its superstates.
     */
    auto is_in( State_Event_Handler_Reference state ) noexcept -> bool
    {
        auto current_state = m_current_state;
        for ( ;; ) {
            if ( current_state == state ) {
                return true;
            } // if

            switch ( ( *current_state )( *this, Discovery::instance() ) ) {
                case Event_Handling_Result::EVENT_IGNORED: return false;
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE:
                    current_state = m_current_state;
                    break;
                case Event_Handling_Result::EVENT_HANDLED: [[fallthrough]];
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED: [[fallthrough]];
                default: trap_fatal_error();
            } // switch
        }     // for
    }

  private:
    /**
     * \brief Discovery pseudo-event.
     */
    class Discovery final : public Event {
      public:
        /**
         * \brief Get a reference to the discovery pseudo-event instance.
         *
         * \return A reference to the discovery pseudo-event instance.
         */
        static constexpr auto instance() noexcept -> Discovery const &
        {
            return INSTANCE;
        }

        Discovery( Discovery && ) = delete;

        Discovery( Discovery const & ) = delete;

        auto operator=( Discovery && ) = delete;

        auto operator=( Discovery const & ) = delete;

      private:
        /**
         * \brief The discovery pseudo-event instance.
         */
        static Discovery const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Discovery() noexcept :
            Event{ Pseudo_Event_Category::instance(), static_cast<Event_ID>( Pseudo_Event::DISCOVERY ) }
        {
        }

        /**
         * \brief Destructor.
         */
        ~Discovery() noexcept = default;
    };

    /**
     * \brief Entry pseudo-event.
     */
    class Entry final : public Event {
      public:
        /**
         * \brief Get a reference to the entry pseudo-event instance.
         *
         * \return A reference to the entry pseudo-event instance.
         */
        static constexpr auto instance() noexcept -> Entry const &
        {
            return INSTANCE;
        }

        Entry( Entry && ) = delete;

        Entry( Entry const & ) = delete;

        auto operator=( Entry && ) = delete;

        auto operator=( Entry const & ) = delete;

      private:
        /**
         * \brief The entry pseudo-event instance.
         */
        static Entry const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Entry() noexcept :
            Event{ Pseudo_Event_Category::instance(), static_cast<Event_ID>( Pseudo_Event::ENTRY ) }
        {
        }

        /**
         * \brief Destructor.
         */
        ~Entry() noexcept = default;
    };

    /**
     * \brief Exit pseudo-event.
     */
    class Exit final : public Event {
      public:
        /**
         * \brief Get a reference to the exit pseudo-event instance.
         *
         * \return A reference to the exit pseudo-event instance.
         */
        static constexpr auto instance() noexcept -> Exit const &
        {
            return INSTANCE;
        }

        Exit( Exit && ) = delete;

        Exit( Exit const & ) = delete;

        auto operator=( Exit && ) = delete;

        auto operator=( Exit const & ) = delete;

      private:
        /**
         * \brief The exit pseudo-event instance.
         */
        static Exit const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Exit() noexcept :
            Event{ Pseudo_Event_Category::instance(), static_cast<Event_ID>( Pseudo_Event::EXIT ) }
        {
        }

        /**
         * \brief Destructor.
         */
        ~Exit() noexcept = default;
    };

    /**
     * \brief Nested initial transition pseudo-event.
     */
    class Nested_Initial_Transition final : public Event {
      public:
        /**
         * \brief Get a reference to the nested initial transition pseudo-event instance.
         *
         * \return A reference to the nested initial transition pseudo-event instance.
         */
        static constexpr auto instance() noexcept -> Nested_Initial_Transition const &
        {
            return INSTANCE;
        }

        Nested_Initial_Transition( Nested_Initial_Transition && ) = delete;

        Nested_Initial_Transition( Nested_Initial_Transition const & ) = delete;

        auto operator=( Nested_Initial_Transition && ) = delete;

        auto operator=( Nested_Initial_Transition const & ) = delete;

      private:
        /**
         * \brief The nested initial transition pseudo-event instance.
         */
        static Nested_Initial_Transition const INSTANCE;

        /**
         * \brief Constructor.
         */
        constexpr Nested_Initial_Transition() noexcept :
            Event{ Pseudo_Event_Category::instance(),
                   static_cast<Event_ID>( Pseudo_Event::NESTED_INITIAL_TRANSITION ) }
        {
        }

        /**
         * \brief Destructor.
         */
        ~Nested_Initial_Transition() noexcept = default;
    };

    /**
     * \brief Max state depth (not including the implicit "top" superstate).
     */
    static constexpr auto MAX_STATE_DEPTH = std::size_t{ 8 };

    /**
     * \brief State path.
     */
    using Path = Fixed_Size_Array<State_Event_Handler_Pointer, MAX_STATE_DEPTH>;

    /**
     * \brief The state event handler for the current state.
     */
    State_Event_Handler_Pointer m_current_state{};

    union {
        /**
         * \brief The state event handler for a superstate.
         */
        State_Event_Handler_Pointer m_superstate{};

        /**
         * \brief The state event handler for the target of a state transition.
         */
        State_Event_Handler_Pointer m_target_state;
    };

    /**
     * \brief Report that an event has been ignored.
     *
     * \param[in] event The event that has been ignored.
     *
     * \return Event ignored event handling result.
     */
    constexpr auto event_ignored( Event const & event ) const noexcept -> Event_Handling_Result
    {
        static_cast<void>( event );

        return Event_Handling_Result::EVENT_IGNORED;
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_HSM_H
