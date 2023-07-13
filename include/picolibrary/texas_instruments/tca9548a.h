/**
 * picolibrary
 *
 * Copyright 2020-2023, Andrew Countryman <apcountryman@gmail.com> and the picolibrary
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
 * \brief picolibrary::Texas_Instruments::TCA9548A interface.
 */

#ifndef PICOLIBRARY_TEXAS_INSTRUMENTS_TCA9548A_H
#define PICOLIBRARY_TEXAS_INSTRUMENTS_TCA9548A_H

#include <cstdint>

/**
 * \brief Texas Instruments TCA9548A facilities.
 */
namespace picolibrary::Texas_Instruments::TCA9548A {

/**
 * \brief Control register.
 */
struct Control {
    /**
     * \brief Register reset value.
     */
    static constexpr auto RESET = std::uint8_t{ 0b00000000 };
};

} // namespace picolibrary::Texas_Instruments::TCA9548A

#endif // PICOLIBRARY_TEXAS_INSTRUMENTS_TCA9548A_H
