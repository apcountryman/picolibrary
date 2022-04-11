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

} // namespace picolibrary::Interrupt

#endif // PICOLIBRARY_INTERRUPT_H
