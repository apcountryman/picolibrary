/**
 * picolibrary
 *
 * Copyright 2020 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Fixed_Size_Array interface.
 */

#ifndef PICOLIBRARY_FIXED_SIZE_ARRAY_H
#define PICOLIBRARY_FIXED_SIZE_ARRAY_H

#include <cstddef>

namespace picolibrary {

/**
 * \brief Fixed size array.
 *
 * \tparam T The array element type.
 * \tparam N The number of elements in the array.
 */
template<typename T, std::size_t N>
class Fixed_Size_Array {
  public:
    /**
     * \brief The array element type.
     */
    using Value = T;

    /**
     * \brief The underlying array.
     */
    Value m_array[ N ];
};

} // namespace picolibrary

#endif // PICOLIBRARY_FIXED_SIZE_ARRAY_H
