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
 * \brief picolibrary::Circular_Buffer interface.
 */

#ifndef PICOLIBRARY_CIRCULAR_BUFFER_H
#define PICOLIBRARY_CIRCULAR_BUFFER_H

namespace picolibrary {

/**
 * \brief Circular buffer use case.
 *
 * \attention These use cases assume that interrupts that interact with a circular buffer
 *            cannot be interrupted by other interrupts that interact with the same
 *            circular buffer.
 */
enum class Circular_Buffer_Use_Case {
    /**
     * The main thread of execution reads from and writes to the circular buffer.
     * Interrupts do not interact with the circular buffer.
     */
    MAIN_READS_WRITES,

    /**
     * An interrupt reads from and writes to the circular buffer. Other interrupts do not
     * interact with the circular buffer. The main thread of execution does not interact
     * with the circular buffer.
     */
    INTERRUPT_READS_WRITES,

    /**
     * An interrupt reads from the circular buffer. One or more other interrupts write to
     * the circular buffer. The main thread of execution does not interact with the
     * circular buffer.
     */
    INTERRUPT_READS_INTERRUPT_WRITES,

    /**
     * The main thread of execution reads from the circular buffer. One or more interrupts
     * write to the circular buffer.
     */
    MAIN_READS_INTERRUPT_WRITES,

    /**
     * An interrupt reads from the circular buffer. Other interrupts do not interact with
     * the circular buffer. The main thread of execution writes to the circular buffer.
     */
    INTERRUPT_READS_MAIN_WRITES,

    /**
     * The main thread of execution reads from and writes to the circular buffer. One or
     * more interrupts write to the circular buffer.
     */
    MAIN_READS_WRITES_INTERRUPT_WRITES,

    /**
     * An interrupt reads from and writes to the circular buffer. Other interrupts may
     * write to the circular buffer. The main thread of execution writes to the circular
     * buffer.
     */
    INTERRUPT_READS_WRITES_MAIN_WRITES,
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in a
 *         circular buffer.
 * \tparam N The maximum number of elements in a circular buffer (must be a power of two).
 * \tparam USE_CASE The circular buffer use case.
 * \tparam Interrupt_Controller The type of interrupt controller used by the main thread
 *         of execution to manipulate the interrupt enable state of any interrupts that
 *         also interact with the circular buffer (must be void if either only the main
 *         thread of execution interacts with a circular buffer or only interrupts
 *         interact with a circular buffer).
 */
template<typename T, typename Size_Type, Size_Type N, Circular_Buffer_Use_Case USE_CASE, typename Interrupt_Controller = void>
class Circular_Buffer;

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
