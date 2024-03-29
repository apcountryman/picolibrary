/**
 * picolibrary
 *
 * Copyright 2020-2024, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
#include <cstdint>
#include <type_traits>

#include "picolibrary/algorithm.h"
#include "picolibrary/error.h"
#include "picolibrary/event.h"
#include "picolibrary/fixed_capacity_vector.h"
#include "picolibrary/precondition.h"
#include "picolibrary/rom.h"

namespace picolibrary {

/**
 * \brief HSM.
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

        /**
         * \brief Get the name of the pseudo-event category.
         *
         * \return The name of the pseudo-event category.
         */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        auto name() const noexcept -> ROM::String override final
        {
            return PICOLIBRARY_ROM_STRING( "::picolibrary::HSM::Pseudo_Event" );
        }
#endif // PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION

        /**
         * \brief Get a pseudo-event's description.
         *
         * \param[in] id The ID of the pseudo-event whose description is to be got.
         *
         * \return The pseudo-event's description.
         */
#ifndef PICOLIBRARY_SUPPRESS_HUMAN_READABLE_EVENT_INFORMATION
        auto event_description( Event_ID id ) const noexcept -> ROM::String override final
        {
            switch ( static_cast<Pseudo_Event>( id ) ) {
                    // clang-format off

                case Pseudo_Event::DISCOVERY: return PICOLIBRARY_ROM_STRING( "DISCOVERY" );
                case Pseudo_Event::ENTRY: return PICOLIBRARY_ROM_STRING( "ENTRY" );
                case Pseudo_Event::EXIT: return PICOLIBRARY_ROM_STRING( "EXIT" );
                case Pseudo_Event::NESTED_INITIAL_TRANSITION: return PICOLIBRARY_ROM_STRING( "NESTED_INITIAL_TRANSITION" );

                    // clang-format on
            } // switch

            return PICOLIBRARY_ROM_STRING( "UNKNOWN" );
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
     *          directly. Use picolibrary::HSM::event_handled(),
     *          picolibrary::HSM::transition_to(), or
     *          picolibrary::HSM::defer_event_handling_to() instead.
     */
    enum class [[nodiscard]] Event_Handling_Result : std::uint_fast8_t{
        EVENT_HANDLED,                         ///< Event handled.
        STATE_TRANSITION_TRIGGERED,            ///< State transition triggered.
        EVENT_HANDLING_DEFERRED_TO_SUPERSTATE, ///< Event handling deferred to superstate.
        EVENT_IGNORED,                         ///< Event ignored.
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
     * if ( &event.category() == &Pseudo_Event_Category::instance() ) {
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::ENTRY ) {
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
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the entry pseudo-event to the
     * state event handler for the implicit "top" superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Exit actions:
     * If a state has exit actions, the state's event handler must execute the state's
     * exit actions when it is passed the exit pseudo-event. Once the state's exit actions
     * have been executed, the state's event handler must report that the exit
     * pseudo-event has been handled. The exit pseudo-event can be identified and acted
     * upon as follows:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance() ) {
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::EXIT ) {
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
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the exit pseudo-event to the state
     * event handler for the implicit "top" superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Nested Initial Transition:
     * If a state has a nested initial transition, the state's event handler must execute
     * the nested initial transition (and any associated actions) when it is passed the
     * nested initial transition pseudo-event. The nested initial transition pseudo-event
     * can be identified and acted upon as follows:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance() ) {
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::NESTED_INITIAL_TRANSITION ) {
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
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the nested initial transition
     * pseudo-event to the state event handler for the implicit "top" superstate as
     * follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Application Event Handling:
     * A state may take any of the following three actions when handling an application
     * event:
     * - Handle the event
     * - Trigger a state transition in response to the event
     * - Defer handling of the event to the state's superstate
     *
     * If the state handles the event, the state's event handler must report that the
     * event has been handled as follows:
     * \code
     * return hsm.event_handled( event );
     * \endcode
     *
     * If the state triggers a state transition in response to the event, the state's
     * event handler must trigger the state transition as follows:
     * \code
     * return hsm.transition_to( target_state, event );
     * \endcode
     *
     * If the state neither handles the event nor triggers a state transition in response
     * to the event, the state's event handler must defer handling of the event to the
     * state event handler for the state's superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( superstate, event );
     * \endcode
     * If the state is a highest level state (a state that has no explicit superstate),
     * the state's event handler must defer handling of the event to the state event
     * handler for the implicit "top" superstate as follows:
     * \code
     * return hsm.defer_event_handling_to( top, event );
     * \endcode
     *
     * Summary:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance() ) {
     *     // entry actions, omit block if the state has no entry actions
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::ENTRY ) {
     *         // execute entry actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     *
     *     // exit actions, omit block if the state has no exit actions
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::EXIT ) {
     *         // execute exit actions
     *
     *         return hsm.event_handled( event );
     *     } // if
     *
     *     // nested initial transition, omit block if the state does not have a nested
     *     // initial transition
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::NESTED_INITIAL_TRANSITION ) {
     *         // execute any actions associated with the nested initial transition
     *
     *         return hsm.transition_to( target_state, event );
     *     } // if
     * } // if
     *
     * // handle application events
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
     * \param[in] handled_event The event that has been handled.
     *
     * \return Event handled event handling result.
     */
    constexpr auto event_handled( Event const & handled_event ) const noexcept -> Event_Handling_Result
    {
        static_cast<void>( handled_event );

        return Event_Handling_Result::EVENT_HANDLED;
    }

    /**
     * \brief Trigger a state transition.
     *
     * \param[in] target_state The state event handler for the target of the state
     *            transition.
     * \param[in] triggering_event The event that is triggering the state transition.
     *
     * \return State transition triggered event handling result.
     */
    constexpr auto transition_to( State_Event_Handler_Reference target_state, Event const & triggering_event ) noexcept
        -> Event_Handling_Result
    {
        static_cast<void>( triggering_event );

        m_target_state = &target_state;

        return Event_Handling_Result::STATE_TRANSITION_TRIGGERED;
    }

    /**
     * \brief Defer handling of an event to a superstate.
     *
     * \param[in] superstate The state event handler for the superstate event handling is
     *            being deferred to.
     * \param[in] deferred_event The event whose handling is to be deferred.
     *
     * \return Event handling deferred to superstate event handling result.
     */
    constexpr auto defer_event_handling_to( State_Event_Handler_Reference superstate, Event const & deferred_event ) noexcept
        -> Event_Handling_Result
    {
        static_cast<void>( deferred_event );

        m_superstate = &superstate;

        return Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE;
    }

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
     *            initial transition by calling picolibrary::HSM::transition_to().
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
     * \brief Destructor.
     */
    ~HSM() noexcept = default;

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
     * \pre this function has not been called previously
     *
     * \pre state event handlers are well behaved
     *
     * \param[in] event The event to pass to the initial pseudostate's event handler to
     *            trigger the initial transition.
     */
    void execute_topmost_initial_transition( Event const & event ) noexcept
    {
        PICOLIBRARY_EXPECT( not m_current_state and m_initial_pseudostate, Generic_Error::LOGIC_ERROR );

        PICOLIBRARY_EXPECT(
            ( *m_initial_pseudostate )( *this, event ) == Event_Handling_Result::STATE_TRANSITION_TRIGGERED,
            Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
        m_current_state = top;
        transition_from( top );
    }

    /**
     * \brief Dispatch an event to the event handler for the currently active state.
     *
     * \pre picolibrary::HSM::execute_topmost_initial_transition() has been called
     *
     * \pre state event handlers are well behaved
     *
     * \param[in] event The event to dispatch to the event handler for the currently
     *            active state.
     */
    void dispatch( Event const & event ) noexcept
    {
        PICOLIBRARY_EXPECT( m_current_state, Generic_Error::LOGIC_ERROR );

        auto state = m_current_state;
        for ( ;; ) {
            switch ( ( *state )( *this, event ) ) {
                case Event_Handling_Result::EVENT_HANDLED: return;
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                    transition_from( state );
                    return;
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE:
                    state = m_superstate;
                    break;
                case Event_Handling_Result::EVENT_IGNORED: return;
                default:
                    PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
            } // switch
        }     // for
    }

    /**
     * \brief Get the state event handler for the currently active state.
     *
     * \return The state event handler for the currently active state.
     */
    constexpr auto current_state() const noexcept -> State_Event_Handler_Pointer
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
    auto is_in( State_Event_Handler_Reference state ) noexcept -> bool
    {
        Path path;

        path.discover( *this, *m_current_state, state );

        return path.is_complete();
    }

  private:
    /**
     * \brief State path.
     */
    class Path {
      public:
        /**
         * \brief Remove the state event handlers for the superstates that are common to a
         *        pair of paths.
         *
         * \param[in] first The first of the two paths.
         * \param[in] second The second of the two paths.
         */
        static void remove_common_superstates( Path & first, Path & second ) noexcept
        {
            while ( first.m_storage.back() == second.m_storage.back() ) {
                first.m_storage.pop_back();
                second.m_storage.pop_back();
            } // while
        }

        /**
         * \brief Constructor.
         */
        constexpr Path() noexcept = default;

        Path( Path && ) = delete;

        Path( Path const & ) = delete;

        /**
         * \brief Destructor.
         */
        ~Path() noexcept = default;

        auto operator=( Path && ) = delete;

        auto operator=( Path const & ) = delete;

        /**
         * \brief Discover a path.
         *
         * \pre state event handlers are well behaved
         *
         * \param[in] hsm The HSM for which a path is being discovered.
         * \param[in] begin The state event handler for the state at the beginning of the
         *            path.
         * \param[in] end The state event handler for the state at the end of the path.
         *
         * If end is top, the path will cover the range [begin,top) and will be considered
         * to be complete.
         *
         * If end is a superstate of begin, the path will cover the range [begin,end) and
         * will be considered to be complete.
         *
         * If end is not a superstate of begin, the path will cover the range [begin,top)
         * and will not be considered to be complete.
         */
        void discover( HSM & hsm, State_Event_Handler_Reference begin, State_Event_Handler_Reference end ) noexcept
        {
            m_is_complete = end == top;
            m_storage.push_back( &begin );

            PICOLIBRARY_EXPECT(
                ( begin )( hsm, DISCOVERY ) == Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE,
                Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
            while ( hsm.m_superstate != top ) {
                if ( hsm.m_superstate == &end ) {
                    m_is_complete = true;

                    return;
                } // if

                m_storage.push_back( hsm.m_superstate );

                PICOLIBRARY_EXPECT(
                    ( *hsm.m_superstate )( hsm, DISCOVERY )
                        == Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE,
                    Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
            } // while
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
        auto begin() const noexcept
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
        auto end() const noexcept
        {
            return m_storage.end();
        }

        /**
         * \brief Get an iterator to the first element of the reversed path.
         *
         * \return An iterator to the first element of the reversed path.
         */
        auto rbegin() const noexcept
        {
            return m_storage.rbegin();
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
        auto rend() const noexcept
        {
            return m_storage.rend();
        }

      private:
        /**
         * \brief Max state depth (not including the implicit "top" superstate).
         */
        static constexpr auto MAX_STATE_DEPTH = std::size_t{ 8 };

        /**
         * \brief The path storage.
         */
        Fixed_Capacity_Vector<State_Event_Handler_Pointer, MAX_STATE_DEPTH> m_storage{};

        /**
         * \brief The path state.
         */
        bool m_is_complete{};
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
         * \brief The state event handler for the target of a state transition.
         */
        State_Event_Handler_Pointer m_target_state;

        /**
         * \brief The state event handler for a superstate.
         */
        State_Event_Handler_Pointer m_superstate;
    };

    /**
     * \brief Report that an event has been ignored.
     *
     * \param[in] ignored_event The event that has been ignored.
     *
     * \return Event ignored event handling result.
     */
    constexpr auto event_ignored( Event const & ignored_event ) const noexcept -> Event_Handling_Result
    {
        static_cast<void>( ignored_event );

        return Event_Handling_Result::EVENT_IGNORED;
    }

    /**
     * \brief Execute a state's entry actions.
     *
     * \param[in] state The state event handler for the state whose entry actions are to
     *            be executed.
     *
     * \pre the state event handler is well behaved
     */
    void enter( State_Event_Handler_Reference state ) noexcept
    {
        switch ( ( state )( *this, ENTRY ) ) {
            case Event_Handling_Result::EVENT_HANDLED: [[fallthrough]];
            case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
            default:
                PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
        } // switch
    }

    /**
     * \brief Execute entry actions for all states in a state path.
     *
     * \param[in] path The state path containing the state event handlers for the states
     *            whose entry actions are to be executed.
     *
     * \pre state event handlers are well behaved
     */
    void enter( Path const & path ) noexcept
    {
        ::picolibrary::for_each( path.rbegin(), path.rend(), [ this ]( auto state ) noexcept {
            enter( *state );
        } );
    }

    /**
     * \brief Execute a state's exit actions.
     *
     * \param[in] state The state event handler for the state whose exit actions are to be
     *            executed.
     *
     * \pre the state event handler is well behaved
     */
    void exit( State_Event_Handler_Reference state ) noexcept
    {
        switch ( ( state )( *this, EXIT ) ) {
            case Event_Handling_Result::EVENT_HANDLED: [[fallthrough]];
            case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
            default:
                PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
        } // switch
    }

    /**
     * \brief Execute exit actions for all states in a state path.
     *
     * \param[in] path The state path containing the state event handlers for the states
     *            whose exit actions are to be executed.
     *
     * \pre state event handlers are well behaved
     */
    void exit( Path const & path ) noexcept
    {
        ::picolibrary::for_each(
            path.begin(), path.end(), [ this ]( auto state ) noexcept { exit( *state ); } );
    }

    /**
     * \brief Execute a state transition and any associated initial transitions.
     *
     * \pre state event handlers are well behaved
     *
     * \param[in] source_state The source state of the state transition.
     */
    // NOLINTNEXTLINE(readability-function-size)
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
                    Path::remove_common_superstates( source_path, target_path );
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
                case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                    source_state = current_state;
                    target_state = m_target_state;
                    break;
                case Event_Handling_Result::EVENT_HANDLING_DEFERRED_TO_SUPERSTATE: return;
                default:
                    PICOLIBRARY_EXPECTATION_NOT_MET( Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
            } // switch
        }     // for
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_HSM_H
