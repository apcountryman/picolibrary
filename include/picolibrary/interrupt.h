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
     * \brief Interrupt(s) enable state.
     */
    struct Interrupt_Enable_State {
    };

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
     *
     * \return The current interrupt(s) enable state.
     */
    auto save_interrupt_enable_state() const noexcept -> Interrupt_Enable_State;

    /**
     * \brief Restore the previously saved interrupt(s) enable state.
     *
     * \param[in] interrupt_enable_state The previously saved interrupt(s) enable state.
     */
    void restore_interrupt_enable_state( Interrupt_Enable_State interrupt_enable_state ) noexcept;
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
 * \brief RAII critical section guard implementation.
 *
 * \tparam Controller The type of controller used to manipulate the interrupt enable
 *         state.
 * \tparam Exit_Action The action to take when exiting the critical section
 *         (picolibrary::Interrupt::Restore_Interrupt_Enable_State or
 *         picolibrary::Interrupt::Enable_Interrupt).
 */
template<typename Controller, typename Exit_Action>
class Critical_Section_Guard_Implementation;

/**
 * \brief RAII critical section guard restore interrupt enable state exit action
 *        implementation.
 *
 * \tparam Controller The type of controller used to manipulate the interrupt enable
 *         state.
 */
template<typename Controller>
class Critical_Section_Guard_Implementation<Controller, Restore_Interrupt_Enable_State> {
  public:
    Critical_Section_Guard_Implementation() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to manipulate the interrupt enable state.
     */
    Critical_Section_Guard_Implementation( Controller & controller ) noexcept :
        m_controller{ controller },
        m_interrupt_enable_state{ controller.save_interrupt_enable_state() }
    {
        m_controller.disable_interrupt();
    }

    Critical_Section_Guard_Implementation( Critical_Section_Guard_Implementation && ) = delete;

    Critical_Section_Guard_Implementation( Critical_Section_Guard_Implementation const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Critical_Section_Guard_Implementation() noexcept
    {
        m_controller.restore_interrupt_enable_state( m_interrupt_enable_state );
    }

    auto operator=( Critical_Section_Guard_Implementation && ) = delete;

    auto operator=( Critical_Section_Guard_Implementation const & ) = delete;

  private:
    /**
     * \brief The controller used to manipulate the interrupt enable state.
     */
    Controller & m_controller;

    /**
     * \brief The saved interrupt enable state.
     */
    typename Controller::Interrupt_Enable_State m_interrupt_enable_state;
};

/**
 * \brief RAII critical section guard enable interrupt exit action implementation.
 *
 * \tparam Controller The type of controller used to manipulate the interrupt enable
 *         state.
 */
template<typename Controller>
class Critical_Section_Guard_Implementation<Controller, Enable_Interrupt> {
  public:
    Critical_Section_Guard_Implementation() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to manipulate the interrupt enable state.
     */
    Critical_Section_Guard_Implementation( Controller & controller ) noexcept :
        m_controller{ controller }
    {
        m_controller.disable_interrupt();
    }

    Critical_Section_Guard_Implementation( Critical_Section_Guard_Implementation && ) = delete;

    Critical_Section_Guard_Implementation( Critical_Section_Guard_Implementation const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Critical_Section_Guard_Implementation() noexcept
    {
        m_controller.enable_interrupt();
    }

    auto operator=( Critical_Section_Guard_Implementation && ) = delete;

    auto operator=( Critical_Section_Guard_Implementation const & ) = delete;

  private:
    /**
     * \brief The controller used to manipulate the interrupt enable state.
     */
    Controller & m_controller;
};

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
class Critical_Section_Guard :
    private Critical_Section_Guard_Implementation<Controller, Exit_Action> {
  public:
    Critical_Section_Guard() = delete;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to manipulate the interrupt enable state.
     */
    Critical_Section_Guard( Controller & controller, Exit_Action ) noexcept :
        Critical_Section_Guard_Implementation<Controller, Exit_Action>{ controller }
    {
    }

    Critical_Section_Guard( Critical_Section_Guard && ) = delete;

    Critical_Section_Guard( Critical_Section_Guard const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Critical_Section_Guard() noexcept = default;

    auto operator=( Critical_Section_Guard && ) = delete;

    auto operator=( Critical_Section_Guard const & ) = delete;
};

} // namespace picolibrary::Interrupt

#endif // PICOLIBRARY_INTERRUPT_H
