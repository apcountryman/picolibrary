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
 * \brief picolibrary::Circular_Buffer interface.
 */

#ifndef PICOLIBRARY_CIRCULAR_BUFFER_H
#define PICOLIBRARY_CIRCULAR_BUFFER_H

#include <cstddef>

namespace picolibrary {

/**
 * \brief picolibrary::Circular_Buffer interrupts supported interrupt support policy.
 */
struct With_Interrupt_Support {
};

/**
 * \brief picolibrary::Circular_Buffer interrupts not supported interrupt support policy.
 */
struct Without_Interrupt_Support {
};

/**
 * \brief picolibrary::Circular_Buffer overflow/underflow protection enabled policy.
 */
struct With_Overflow_Underflow_Protection {
};

/**
 * \brief picolibrary::Circular_Buffer overflow/underflow protection disabled policy.
 */
struct Without_Overflow_Underflow_Protection {
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam N The maximum number of elements in the circular buffer.
 * \tparam Interrupt_Support The circular buffer interrupt support policy
 *         (picolibrary::With_Interrupt_Support or
 *         picolibrary::Without_Interrupt_Support).
 * \tparam Overflow_Underflow_Protection The circular buffer overflow/underflow protection
 *         policy (picolibrary::With_Overflow_Underflow_Protection or
 *         picolibrary::Without_Overflow_Underflow_Protection).
 */
template<typename T, std::size_t N, typename Interrupt_Support, typename Overflow_Underflow_Protection>
class Circular_Buffer;

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
