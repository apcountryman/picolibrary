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
 * \brief picolibrary::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP23008_H
#define PICOLIBRARY_MICROCHIP_MCP23008_H

#include "picolibrary/i2c.h"
#include "picolibrary/precondition.h"

/**
 * \brief Microchip MCP23008 facilities.
 */
namespace picolibrary::Microchip::MCP23008 {

/**
 * \brief Get the minimum valid address.
 *
 * \return The minimum valid address.
 */
constexpr auto address_min() noexcept
{
    return I2C::Address_Transmitted{ I2C::Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                           0b0100'000 } };
}

/**
 * \brief Get the maximum valid address.
 *
 * \return The maximum valid address.
 */
constexpr auto address_max() noexcept
{
    return I2C::Address_Transmitted{ I2C::Address_Numeric{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                                                           0b0100'111 } };
}

} // namespace picolibrary::Microchip::MCP23008

#endif // PICOLIBRARY_MICROCHIP_MCP23008_H
