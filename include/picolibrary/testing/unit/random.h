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
 * \brief picolibrary::Testing::Unit random interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_RANDOM_H
#define PICOLIBRARY_TESTING_UNIT_RANDOM_H

#include <random>

namespace picolibrary::Testing::Unit {

/**
 * \brief Get the unit testing pseudo-random number generator.
 *
 * \return The unit testing pseudo-random number generator.
 */
inline auto & pseudo_random_number_generator()
{
    static auto generator = std::mt19937{ std::random_device{}() };

    return generator;
}

} // namespace picolibrary::Testing::Unit

#endif // PICOLIBRARY_TESTING_UNIT_RANDOM_H
