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
 * \brief picolibrary::Testing::Unit::Indicator interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_INDICATOR_H
#define PICOLIBRARY_TESTING_UNIT_INDICATOR_H

#include "picolibrary/indicator.h"
#include "picolibrary/testing/unit/random.h"

namespace picolibrary::Testing::Unit {

/**
 * \brief Generate a pseudo-random picolibrary::Indicator::Initial_Indicator_State.
 *
 * \return A pseudo-randomly generated picolibrary::Indicator::Initial_Indicator_State.
 */
template<>
inline auto random<Indicator::Initial_Indicator_State>()
{
    return random<bool>() ? Indicator::Initial_Indicator_State::ILLUMINATED
                          : Indicator::Initial_Indicator_State::EXTINGUISHED;
}

} // namespace picolibrary::Testing::Unit

/**
 * \brief Indicator unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Indicator {
} // namespace picolibrary::Testing::Unit::Indicator

#endif // PICOLIBRARY_TESTING_UNIT_INDICATOR_H
