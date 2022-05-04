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
 * \brief picolibrary::Testing::Automated::Microchip::MCP23008 interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23008_H
#define PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23008_H

#include <functional>

#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp23008.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/testing/automated/microchip/mcp23x08.h"

/**
 * \brief Microchip MCP23008 automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Microchip::MCP23008 {

/**
 * \brief Mock driver.
 */
class Mock_Driver : public I2C::Mock_Device, public MCP23X08::Mock_Driver {
  public:
    Mock_Driver() = default;

    Mock_Driver( std::function<void()>, I2C::Mock_Controller &, ::picolibrary::Microchip::MCP23008::Address_Transmitted, Error_Code const & )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;
};

} // namespace picolibrary::Testing::Automated::Microchip::MCP23008

#endif // PICOLIBRARY_TESTING_AUTOMATED_MICROCHIP_MCP23008_H
