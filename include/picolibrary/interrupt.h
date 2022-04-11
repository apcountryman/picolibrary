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
 * \brief picolibrary::Interrupt interface.
 */

#ifndef PICOLIBRARY_INTERRUPT_H
#define PICOLIBRARY_INTERRUPT_H

#include <type_traits>

/**
 * \brief Interrupt facilities.
 */
namespace picolibrary::Interrupt {

/**
 * \brief Controller concept.
 */
class Controller_Concept {
  public:
    /**
     * \brief Constructor.
     */
    Controller_Concept() noexcept;

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    Controller_Concept( Controller_Concept && source ) noexcept;

    Controller_Concept( Controller_Concept const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Controller_Concept() noexcept;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    auto operator=( Controller_Concept && expression ) noexcept -> Controller_Concept &;

    auto operator=( Controller_Concept const & ) = delete;

    /**
     * \brief Disable the interrupt(s).
     */
    void disable_interrupt() noexcept;

    /**
     * \brief Enable the interrupts(s).
     */
    void enable_interrupt() noexcept;

    /**
     * \brief Save the current interrupt(s) enable state.
     */
    void save_interrupt_enable_state() noexcept;

    /**
     * \brief Restore the previously saved interrupt(s) enable state.
     */
    void restore_interrupt_enable_state() noexcept;
};

/**
 * \brief Restore interrupt enable state critical section exit action.
 */
struct Restore_Interrupt_Enable_State {
};

/**
 * \brief Restore interrupt enable state critical section exit action.
 */
constexpr auto RESTORE_INTERRUPT_ENABLE_STATE = Restore_Interrupt_Enable_State{};

/**
 * \brief Enable interrupt critical section exit action.
 */
struct Enable_Interrupt {
};

/**
 * \brief Enable interrupt critical section exit action.
 */
constexpr auto ENABLE_INTERRUPT = Enable_Interrupt{};

/**
 * \brief RAII critical section guard.
 *
 * \tparam Controller The type of controller used to manipulate the interrupt enable
 *         state.
 * \tparam Exit_Action The action to take when exiting the critical section
 *         (picolibrary::Interrupt::Restore_Interrupt_Enable_State or
 *         picolibrary::Interrupt::Enable_Interrupt).
 */
template<typename Controller, typename Exit_Action>
class Critical_Section_Guard {
  public:
    static_assert( std::is_same_v<Exit_Action, Restore_Interrupt_Enable_State> or std::is_same_v<Exit_Action, Enable_Interrupt> );

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to manipulate the interrupt enable state.
     */
    Critical_Section_Guard( Controller & controller, Exit_Action ) noexcept :
        m_controller{ controller }
    {
        enter( Exit_Action{} );
    }

    Critical_Section_Guard( Critical_Section_Guard && ) = delete;

    Critical_Section_Guard( Critical_Section_Guard const & ) = delete;

    ~Critical_Section_Guard() noexcept
    {
        exit( Exit_Action{} );
    }

    auto operator=( Critical_Section_Guard && ) = delete;

    auto operator=( Critical_Section_Guard const & ) = delete;

  private:
    /**
     * \brief The controller used to manipulate the interrupt enable state.
     */
    Controller & m_controller;

    /**
     * \brief Execute the critical section entry actions associated with the restore
     *        interrupt enable state critical section exit action.
     */
    void enter( Restore_Interrupt_Enable_State ) noexcept
    {
        m_controller.save_interrupt_enable_state();
        m_controller.disable_interrupt();
    }

    /**
     * \brief Execute the restore interrupt enable state critical section exit action.
     */
    void exit( Restore_Interrupt_Enable_State ) noexcept
    {
        m_controller.restore_interrupt_enable_state();
    }

    /**
     * \brief Execute the critical section entry action associated with the enable
     *        interrupt critical section exit action.
     */
    void enter( Enable_Interrupt ) noexcept
    {
        m_controller.disable_interrupt();
    }

    /**
     * \brief Execute the enable interrupt critical section exit action.
     */
    void exit( Enable_Interrupt ) noexcept
    {
        m_controller.enable_interrupt();
    }
};

} // namespace picolibrary::Interrupt

#endif // PICOLIBRARY_INTERRUPT_H
