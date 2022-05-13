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
 * \brief picolibrary::State_Machine interface.
 */

#ifndef PICOLIBRARY_STATE_MACHINE_H
#define PICOLIBRARY_STATE_MACHINE_H

#include <cstdint>
#include <type_traits>

#include "picolibrary/error.h"
#include "picolibrary/event.h"
#include "picolibrary/precondition.h"

namespace picolibrary {

/**
 * \brief State machine with support for state entry and exit actions.
 */
class State_Machine {
  public:
    /**
     * \brief Pseudo-event.
     */
    enum class Pseudo_Event : Event_ID {
        ENTRY, ///< State entered, execute entry actions.
        EXIT,  ///< State exited, execute exit actions.
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
        virtual auto name() const noexcept -> char const * override final
        {
            return "::picolibrary::State_Machine::Pseudo_Event";
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
        virtual auto event_description( Event_ID id ) const noexcept -> char const * override final
        {
            switch ( static_cast<Pseudo_Event>( id ) ) {
                case Pseudo_Event::ENTRY: return "ENTRY";
                case Pseudo_Event::EXIT: return "EXIT";
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
     *          directly. Use picolibrary::State_Machine::event_handled(), or
     *          picolibrary::State_Machine::transition_to() instead.
     */
    enum class [[nodiscard]] Event_Handling_Result : std::uint_fast8_t{
        EVENT_HANDLED,              ///< Event handled.
        STATE_TRANSITION_TRIGGERED, ///< State transition triggered.
    };

    /**
     * \brief State event handler.
     *
     * \param[in] state_machine The state machine.
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
     *         return state_machine.event_handled( event );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have entry actions, the state's event handler must report that
     * the entry pseudo-event has been handled as follows:
     * \code
     * return state_machine.event_handled( event );
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
     *         return state_machine.event_handled( event );
     *     } // if
     * } // if
     * \endcode
     *
     * If a state does not have exit actions, the state's event handler must report that
     * the exit pseudo-event has been handled as follows:
     * \code
     * return state_machine.event_handled( event );
     * \endcode
     *
     * Application Event Handling:
     * A state may take either of the following actions when handling an application
     * event:
     * - Handle the event
     * - Trigger a state transition in response to the event
     *
     * If the state handles the event, the state's event handler must report that the
     * event has been handled as follows:
     * \code
     * return state_machine.event_handled( event );
     * \endcode
     *
     * If the state triggers a state transition in response to the event, the state's
     * event handler must trigger the state transition as follows:
     * \code
     * return state_machine.transition_to( target_state, event );
     * \endcode
     *
     * Summary:
     * \code
     * if ( &event.category() == &Pseudo_Event_Category::instance() ) {
     *     // entry actions, omit block if the state has no entry actions
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::ENTRY ) {
     *         // execute entry actions
     *
     *         return state_machine.event_handled( event );
     *     } // if
     *
     *     // exit actions, omit block if the state has no exit actions
     *     if ( static_cast<Pseudo_Event>( event.id() ) == Pseudo_Event::EXIT ) {
     *         // execute exit actions
     *
     *         return state_machine.event_handled( event );
     *     } // if
     * } // if
     *
     * // handle application events
     *
     * return state_machine.event_handled( event );
     * \endcode
     */
    using State_Event_Handler = auto( State_Machine & state_machine, Event const & event ) noexcept
                                -> Event_Handling_Result;

    /**
     * \brief State event handler pointer.
     */
    using State_Event_Handler_Pointer = std::add_pointer_t<State_Event_Handler>;

    /**
     * \brief State event handler reference.
     */
    using State_Event_Handler_Reference = std::add_lvalue_reference_t<State_Event_Handler>;

    /**
     * \brief Report that an event has been handled.
     *
     * \param[in] handled_event The event that has been handled.
     *
     * \return Event handled event handling result.
     */
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
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

  protected:
    /**
     * \brief Constructor.
     */
    constexpr State_Machine() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] initial_pseudostate The state event handler for the initial pseudostate.
     *            The state event handler for the initial pseudostate must trigger the
     *            initial transition by calling
     *            picolibrary::State_Machine::transition_to().
     */
    constexpr State_Machine( State_Event_Handler_Reference initial_pseudostate ) noexcept :
        m_initial_pseudostate{ &initial_pseudostate }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr State_Machine( State_Machine && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr State_Machine( State_Machine const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~State_Machine() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( State_Machine && expression ) noexcept -> State_Machine & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( State_Machine const & expression ) noexcept -> State_Machine & = default;

    /**
     * \brief Execute the initial transition.
     *
     * \pre this function has not been called previously
     *
     * \pre state event handlers are well behaved
     *
     * \param[in] event The event to pass to the initial pseudostate's event handler to
     *            trigger the initial transition.
     */
    void execute_initial_transition( Event const & event ) noexcept
    {
        expect( not m_current_state and m_initial_pseudostate, Generic_Error::LOGIC_ERROR );

        expect(
            ( *m_initial_pseudostate )( *this, event ) == Event_Handling_Result::STATE_TRANSITION_TRIGGERED,
            Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
        enter( *m_target_state );
        m_current_state = m_target_state;
    }

    /**
     * \brief Dispatch an event to the event handler for the currently active state.
     *
     * \pre picolibrary::State_Machine::execute_initial_transition() has been called
     *
     * \pre state event handlers are well behaved
     *
     * \param[in] event The event to dispatch to the event handler for the currently
     *            active state.
     */
    void dispatch( Event const & event ) noexcept
    {
        expect( m_current_state, Generic_Error::LOGIC_ERROR );

        switch ( ( *m_current_state )( *this, event ) ) {
            case Event_Handling_Result::EVENT_HANDLED: return;
            case Event_Handling_Result::STATE_TRANSITION_TRIGGERED:
                exit( *m_current_state );
                enter( *m_target_state );
                m_current_state = m_target_state;
                return;
            default: expect( false, Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
        } // switch
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
     *        active state.
     *
     * \param[in] state The state event handler to check if it is the state event handler
     *            for the currently active state.
     *
     * \return true if state is the state event handler for the currently active state.
     * \return false if state is not the state event handler for the currently active
     *         state.
     */
    constexpr auto is_in( State_Event_Handler_Reference state ) noexcept -> bool
    {
        return m_current_state == &state;
    }

  private:
    /**
     * \brief Entry pseudo-event.
     */
    static Simple_Event const ENTRY;

    /**
     * \brief Exit pseudo-event.
     */
    static Simple_Event const EXIT;

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
    };

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
        expect(
            ( state )( *this, ENTRY ) == Event_Handling_Result::EVENT_HANDLED,
            Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
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
        expect(
            ( state )( *this, EXIT ) == Event_Handling_Result::EVENT_HANDLED,
            Generic_Error::UNEXPECTED_EVENT_HANDLING_RESULT );
    }
};

} // namespace picolibrary

#endif // PICOLIBRARY_STATE_MACHINE_H
