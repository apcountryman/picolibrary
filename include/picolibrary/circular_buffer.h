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
 * \brief picolibrary::Circular_Buffer single reader/writer access pattern.
 */
struct Single_Reader_Writer {
};

/**
 * \brief picolibrary::Circular_Buffer single reader, single writer access pattern.
 */
struct Single_Reader_Single_Writer {
};

/**
 * \brief picolibrary::Circular_Buffer single reader, multiple writers.
 */
struct Single_Reader_Multiple_Writers {
};

/**
 * \brief Circular buffer.
 *
 * \tparam T The circular buffer element type.
 * \tparam Size_Type The unsigned integer type used to track the number of elements in the
 *         circular buffer.
 * \tparam N The maximum number of elements in the circular buffer.
 * \tparam Access_Pattern The circular buffer access pattern
 *         (picolibrary::Single_Reader_Writer, picolibrary::Single_Reader_Single_Writer,
 *         or picolibrary::Single_Reader_Multiple_Writers).
 * \tparam Interrupt_Controller The type of interrupt controller used to manipulate the
 *         interrupt enable state of any interrupts that interact with the circular buffer
 *         (must be void if Access_Pattern is picolibrary::Single_Reader_Writer).
 */
template<typename T, typename Size_Type, Size_Type N, typename Access_Pattern, typename Interrupt_Controller = void>
class Circular_Buffer;

} // namespace picolibrary

#endif // PICOLIBRARY_CIRCULAR_BUFFER_H
