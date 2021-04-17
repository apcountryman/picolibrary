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
 * \brief picolibrary::Interrupt_Guard interface.
 */

#ifndef PICOLIBRARY_INTERRUPT_GUARD_H
#define PICOLIBRARY_INTERRUPT_GUARD_H

#include <type_traits>

namespace picolibrary {

/**
 * \brief When a picolibrary::Interrupt_Guard is destructed, restore the interrupt enable
 *        state to what is was when the picolibrary::Interrupt_Guard was constructed.
 */
struct Restore_Interrupt_Enable_State {};

/**
 * \brief When a picolibrary::Interrupt_Guard is destructed, enable interrupts.
 */
struct Enable_Interrupts {};

/**
 * \brief When a picolibrary::Interrupt_Guard is destructed, disable interrupts.
 */
struct Disable_Interrupts {};

} // namespace picolibrary

#if __has_include( "picolibrary/hardware/interrupt_guard.h" )
#include "picolibrary/hardware/interrupt_guard.h"
#else // __has_include( "picolibrary/hardware/interrupt_guard.h" )

namespace picolibrary {

/**
 * \biref Interrupt guard.
 *
 * The default interrupt guard implementation does nothing. The default implementation can
 * be replaced with a hardware specific implementation by setting the
 * PICOLIBRARY_HARDWARE_INCLUDE_DIR CMake variable to the directory where hardware
 * specific headers are defined, and implementing the hardware specific implementation in
 * picolibrary/hardware/interrupt_guard.h. A hardware specific implementation should
 * disable interrupts when constructed, and execute the user defined action when
 * destructed.
 *
 * \tparam Action_On_Destruction The action to take
 *         (picolibrary::Restore_Interrupt_Enable_State, picolibrary::Enable_Interrupts,
 *         or picolibrary::Disable_Interrupts) when the picolibrary::Interrupt_Guard is
 *         destructed.
 */
template<typename Action_On_Destruction>
class Interrupt_Guard {
    public:
        static_assert(
                std::is_same_v<Action_On_Destruction, Restore_Interrupt_Enable_State> or
                std::is_same_v<Action_On_Destruction, Enable_Interrupts> or
                std::is_same_v<Action_On_Destruction, Disable_Interrupts> );

    /**
     * \brief Constructor.
     */
    constexpr Interrupt_Guard() noexcept = default;

    Interrupt_Guard( Interrupt_Guard && ) = delete;

    Interrupt_Guard( Interrupt_Guard const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Interrupt_Guard() noexcept = default;

    auto operator=( Interrupt_Guard && ) = delete;

    auto operator=( Interrupt_Guard const & ) = delete;
};

} // namespace picolibrary

#endif // __has_include( "picolibrary/hardware/interrupt_guard.h" )

#endif // PICOLIBRARY_INTERRUPT_GUARD_H
