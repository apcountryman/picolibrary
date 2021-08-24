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
 * \brief picolibrary::State_Machine interface.
 */

#ifndef PICOLIBRARY_STATE_MACHINE_H
#define PICOLIBRARY_STATE_MACHINE_H

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "picolibrary/algorithm.h"
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
     * \brief Pseudo-event.
     */
    enum class Pseudo_Event : Event_ID {
        DISCOVERY, ///< Discovering state hierarchy, defer event handling to superstate.
        ENTRY,     ///< State entered, execute entry actions.
        EXIT,      ///< State exited, execute exit actions.
        NESTED_INITIAL_TRANSITION, ///< State entered, entry actions executed, execute nested initial transition (and any associated actions).
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
        static constexpr auto instance() noexcept -> Pseudo_Event_Category const &
        {
            return INSTANCE;
        }

        Pseudo_Event_Category( Pseudo_Event_Category && ) = delete;

        Pseudo_Event_Category( Pseudo_Event_Category const & ) = delete;

        auto operator=( Pseudo_Event_Category && ) = delete;

        auto operator=( Pseudo_Event_Category const & ) = delete;

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        /**
         * \brief Get the name of the pseudo-event category.
         *
         * \return The name of the pseudo-event category.
         */
        virtual auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::HSM::Pseudo_Event";
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        /**
         * \brief Get a pseudo-event's description.
         *
         * \param[in] id The ID of the pseudo-event whose description is to be got.
         *
         * \return The pseudo-event's description.
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
     *
     * \warning State event handlers should never construct event handling results
     *          directory. Use picolibrary::HSM::event_handled(),
     *          picolibrary::HSM::transition_to(), or
     *          picolibrary::HSM::defer_event_handling_to() instead.
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
     * \param[in] hsm The HSM.
     * \param[in] event The event to handle.
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
     * return hsm.defer_event_handling_to( superstate, event );
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
     * return hsm.defer_event_handling_to( superstate, event );
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
     *         return hsm.transition_to( target_state, event );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have a nested initial transition, the state's event handler
     * must defer handling of the nested initial transition pseudo-event to the state
     * event handler for the state's superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( superstate, event );
     * \endcode
     *
     * Event Handling:
     * If a state handles an event, the state's event handler must report that the event
     * has been handled as follows:
     * \code
     * return hsm.event_handled( event );
     * \endcode
     *
     * Event Handling Deferral:
     * If a state does not explicitly handle an event, the state's event handler must
     * defer handling of the event to the state event handler for the state's superstate
     * as follows:
     * \code
     * return hsm.defer_event_handling_to( superstate, event );
     * \endcode
     *
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the event to the state event
     * handler for the implicit "top" superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Summary:
     * \code
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
     *         return hsm.transition_to( target_state, event );
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
     * \param[in] event The event whose handling is to be deferred.
     *
     * \return Event handling deferred to superstate event handling result.
     */
    constexpr auto defer_event_handling_to( State_Event_Handler_Reference superstate, Event const & event ) noexcept
    {
        static_cast<void>( event );

        m_superstate = &superstate;

        return Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE;
    }

    /**
     * \brief Trigger a state transition.
     *
     * \param[in] target_state The state event handler for the target of the state
     *            transition.
     * \param[in] trigger The event that is triggering the state transition.
     *
     * \return State transition triggered event handling result.
     */
    constexpr auto transition_to( State_Event_Handler_Reference target_state, Event const & trigger ) noexcept
    {
        static_cast<void>( trigger );

        m_target_state = &target_state;

        return Event_Handling_Result::STATE_TRANSITION_TRIGGERED;
    }

    /**
     * \brief Destructor.
     */
    ~HSM() noexcept = default;

  protected:
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
        m_initial_pseudostate{ &initial_pseudostate }
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
     * \brief Execute the topmost initial transition and any resulting nested initial
     *        transitions.
     *
     * \attention This function must be called before picolibrary::HSM::dispatch() is
     *            called.
     *
     * \warning This function must not be called more than once.
     *
     * \param[in] event The event to pass to the initial pseudostate's event handler to
     *            trigger the topmost initial transition.
     */
    void execute_topmost_initial_transition( Event const & event ) noexcept
    {
        if ( ( *m_initial_pseudostate )( *this, event ) != Event_Handling_Result::STATE_TRANSITION_TRIGGERED ) {
            trap_fatal_error();
        } // if

        m_current_state = top;

        transition_from( top );
    }

    /**
     * \brief Dispatch an event to the event handler for the currently active state.
     *
     * \attention picolibrary::HSM::execute_topmost_initial_transition() must be called
     *            before this function is called.
     *
     * \param[in] event The event to dispatch to the event handler for the currently
     *            active state.
     */
    void dispatch( Event const & event ) noexcept
    {
        auto state = m_current_state;
        for ( ;; ) {
            switch ( ( *state )( *this, event ) ) {
                case Event_Handling_Result::EVENT_IGNORED: return;
                case Event_Handling_Result::EVENT_HANDLED: return;
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE:
                    state = m_superstate;
                    break;
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                    transition_from( state );
                    return;
                default: trap_fatal_error();
            } // switch
        }     // for
    }

    /**
     * \brief Get the state event handler for the currently active state.
     *
     * \return The state event handler for the currently active state.
     */
    constexpr auto current_state() const noexcept
    {
        return m_current_state;
    }

    /**
     * \brief Check if a state event handler is the state event handler for the currently
     *        active state or one of its superstates.
     *
     * \param[in] state The state event handler to check if it is the state event handler
     *            for the currently active state or one of its superstates.
     *
     * \return true if state is the state event handler for the currently active state or
     *         one of its superstates.
     * \return false if state is neither the state event handler for the currently active
     *         state nor any of its superstates.
     */
    auto is_in( State_Event_Handler_Reference state ) noexcept
    {
        Path path;

        path.discover( *this, *m_current_state, state );

        return path.is_complete();
    }

  private:
    /**
     * \brief State path.
     */
    class Path;

    /**
     * \brief Remove the state event handlers for the superstates that are common to a
     *        pair of paths.
     *
     * \relatedalso picolibrary::HSM::Path
     *
     * \param[in] first The first of the two paths.
     * \param[in] second The second of the two paths.
     */
    static void remove_common_superstates( Path & first, Path & second ) noexcept;

    class Path {
      public:
        /**
         * \brief Max state depth (not including the implicit "top" superstate).
         */
        static constexpr auto MAX_STATE_DEPTH = std::size_t{ 8 };

        /**
         * \brief Path storage.
         */
        using Storage = Fixed_Size_Array<State_Event_Handler_Pointer, MAX_STATE_DEPTH>;

        /**
         * \brief A const path iterator.
         */
        using Const_Iterator = Storage::Const_Iterator;

        /**
         * \brief A const path reverse iterator.
         */
        using Const_Reverse_Iterator = Storage::Const_Reverse_Iterator;

        /**
         * \brief Constructor.
         */
        Path() noexcept = default;

        Path( Path && ) = delete;

        Path( Path const & ) = delete;

        ~Path() noexcept = default;

        auto operator=( Path && ) = delete;

        auto operator=( Path const & ) = delete;

        /**
         * \brief Discover a path.
         *
         * \param[in] hsm The HSM for which a path is being discovered.
         * \param[in] begin The state event handler for the state at the beginning of the
         *            path.
         * \param[in] end The state event handler for the state at the end of the path.
         *
         * If end is top, the path will cover the range [begin,top) and will be be
         * considered to be complete. If end is a superstate of begin, the path will cover
         * the range [begin,end) and will be considered to be complete. If end is not a
         * superstate of begin, the path will cover the range [begin,top) and will not be
         * considered to be complete.
         */
        void discover( HSM & hsm, State_Event_Handler_Reference begin, State_Event_Handler_Reference end ) noexcept
        {
            // #lizard forgives the length

            m_is_complete  = end == top;
            m_storage[ 0 ] = &begin;
            m_size         = 1;

            if ( ( begin )( hsm, DISCOVERY ) != Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE ) {
                trap_fatal_error();
            } // if
            while ( hsm.m_superstate != top ) {
                if ( hsm.m_superstate == &end ) {
                    m_is_complete = true;
                    return;
                } // if

                if ( m_size >= m_storage.size() ) {
                    trap_fatal_error();
                } // if

                m_storage[ m_size ] = hsm.m_superstate;
                ++m_size;

                if ( ( *hsm.m_superstate )( hsm, DISCOVERY )
                     != Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE ) {
                    trap_fatal_error();
                } // if
            }     // while
        }

        /**
         * \brief Check if the path is complete.
         *
         * \return true if the path is complete.
         * \return false if the path is not complete.
         */
        auto is_complete() const noexcept -> bool
        {
            return m_is_complete;
        }

        /**
         * \brief Get an iterator to the first element of the path.
         *
         * \return An iterator to the first element of the path.
         */
        auto begin() const noexcept -> Const_Iterator
        {
            return m_storage.begin();
        }

        /**
         * \brief Get an iterator to the element following the last element of the path.
         *
         * \warning Attempting to access the element following the last element of a path
         *          results in undefined behavior.
         *
         * \return An iterator to the element following the last element of the path.
         */
        auto end() const noexcept -> Const_Iterator
        {
            return begin() + m_size;
        }

        /**
         * \brief Get an iterator to the first element of the reversed path.
         *
         * \return An iterator to the first element of the reversed path.
         */
        auto rbegin() const noexcept -> Const_Reverse_Iterator
        {
            return Const_Reverse_Iterator{ end() };
        }

        /**
         * \brief Get an iterator to the element following the last element of the
         *        reversed path.
         *
         * \warning Attempting to access the element following the last element of a
         *          reversed path results in undefined behavior.
         *
         * \return An iterator to the element following the last element of the reversed
         *         path.
         */
        auto rend() const noexcept -> Const_Reverse_Iterator
        {
            return Const_Reverse_Iterator{ begin() };
        }

      private:
        /**
         * \brief The path storage.
         */
        Storage m_storage;

        /**
         * \brief The path size.
         */
        Storage::Size m_size{ 0 };

        /**
         * \brief The path state.
         */
        bool m_is_complete{};

        friend void HSM::remove_common_superstates( Path & first, Path & second ) noexcept;

        /**
         * \brief Remove an element from the end of the path.
         */
        void pop() noexcept
        {
            --m_size;
        }

        /**
         * \brief Access the last element of the path.
         *
         * \return The last element of the path.
         */
        auto back() const noexcept
        {
            return *( end() - 1 );
        }
    };

    /**
     * \brief Discovery pseudo-event.
     */
    static Simple_Event const DISCOVERY;

    /**
     * \brief Entry pseudo-event.
     */
    static Simple_Event const ENTRY;

    /**
     * \brief Exit pseudo-event.
     */
    static Simple_Event const EXIT;

    /**
     * \brief Nested initial transition pseudo-event.
     */
    static Simple_Event const NESTED_INITIAL_TRANSITION;

    /**
     * \brief The state event handler for the currently active state.
     */
    State_Event_Handler_Pointer m_current_state{};

    union {
        /**
         * \brief The state event handler for the initial pseudostate.
         */
        State_Event_Handler_Pointer m_initial_pseudostate{};

        /**
         * \brief The state event handler for a superstate.
         */
        State_Event_Handler_Pointer m_superstate;

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

    /**
     * \brief Execute a state's entry actions.
     *
     * \param[in] state The state event handler for the state whose entry actions are to
     *            be executed.
     */
    void enter( State_Event_Handler_Reference state ) noexcept
    {
        switch ( ( state )( *this, ENTRY ) ) {
            case Event_Handling_Result::EVENT_HANDLED: return;
            case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
            default: trap_fatal_error();
        } // switch
    }

    /**
     * \brief Execute entry actions for all states in a state path.
     *
     * \param[in] path The state path containing the state event handlers for the states
     *            whose entry actions are to be executed.
     */
    void enter( Path const & path ) noexcept
    {
        for_each( path.rbegin(), path.rend(), [ this ]( auto state ) noexcept {
            enter( *state );
        } );
    }

    /**
     * \brief Execute a state's exit actions.
     *
     * \param[in] state The state event handler for the state whose exit actions are to be
     *            executed.
     */
    void exit( State_Event_Handler_Reference state ) noexcept
    {
        switch ( ( state )( *this, EXIT ) ) {
            case Event_Handling_Result::EVENT_HANDLED: return;
            case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
            default: trap_fatal_error();
        } // switch
    }

    /**
     * \brief Execute exit actions for all states in a state path.
     *
     * \param[in] path The state path containing the state event handlers for the states
     *            whose exit actions are to be executed.
     */
    void exit( Path const & path ) noexcept
    {
        for_each(
            path.begin(), path.end(), [ this ]( auto state ) noexcept { exit( *state ); } );
    }

    /**
     * \brief Execute a state transition and any associated nested initial transitions.
     *
     * \param[in] source_state The source state of the state transition.
     */
    void transition_from( State_Event_Handler_Pointer source_state ) noexcept
    {
        // #lizard forgives the length

        auto current_state = m_current_state;
        auto target_state  = m_target_state;

        for ( ;; ) {
            if ( current_state != source_state ) {
                auto current_path = Path{};

                current_path.discover( *this, *current_state, *source_state );

                exit( current_path );
            } // if

            if ( target_state == source_state ) {
                exit( *source_state );
                enter( *target_state );
            } else {
                auto target_path = Path{};
                auto source_path = Path{};

                target_path.discover( *this, *target_state, *source_state );

                if ( not target_path.is_complete() ) {
                    source_path.discover( *this, *source_state, *target_state );
                } // if

                if ( not source_path.is_complete() and not target_path.is_complete() ) {
                    remove_common_superstates( source_path, target_path );
                } // if

                if ( not target_path.is_complete() ) {
                    exit( source_path );
                } // if

                if ( not source_path.is_complete() ) {
                    enter( target_path );
                } // if
            }     // else

            current_state   = target_state;
            m_current_state = target_state;

            switch ( ( *current_state )( *this, NESTED_INITIAL_TRANSITION ) ) {
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                    source_state = current_state;
                    target_state = m_target_state;
                    break;
                default: trap_fatal_error();
            } // switch
        }     // for
    }
};

inline void HSM::remove_common_superstates( Path & first, Path & second ) noexcept
{
    while ( first.back() == second.back() ) {
        first.pop();
        second.pop();
    } // while
}

} // namespace picolibrary

#endif // PICOLIBRARY_STATE_MACHINE_H
