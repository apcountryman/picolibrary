/**
 * picolibrary
 *
 * Copyright 2021 Andrew Countryman <apcountryman@gmail.com>
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
 * \brief picolibrary::Testing::Unit::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H
#define PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/microchip/mcp3008.h"
#include "picolibrary/result.h"
#include "picolibrary/testing/unit/spi.h"
#include "picolibrary/utility.h"

/**
 * \brief Microchip MCP3008 unit testing facilities.
 */
namespace picolibrary::Testing::Unit::Microchip::MCP3008 {

/**
 * \brief Mock Microchip MCP3008 driver.
 */
class Mock_Driver {
  public:
    /**
     * \copydoc picolibrary::Microchip::MCP3008::Driver::Controller
     */
    using Controller = SPI::Mock_Controller;

    /**
     * \copydoc picolibrary::Microchip::MCP3008::Driver::Device_Selector
     */
    using Device_Selector = SPI::Mock_Device_Selector::Handle;

    /**
     * \brief Constructor.
     */
    Mock_Driver() = default;

    /**
     * \brief Constructor.
     */
    Mock_Driver( Controller &, Controller::Configuration, Device_Selector, Error_Code )
    {
    }

    /**
     * \todo #29
     */
    Mock_Driver( Mock_Driver && ) = delete;

    /**
     * \todo #29
     */
    Mock_Driver( Mock_Driver const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Mock_Driver() noexcept = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Driver && ) = delete;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( (Result<Void, Error_Code>), initialize, () );

    MOCK_METHOD(
        (Result<::picolibrary::Microchip::MCP3008::Sample, Error_Code>),
        sample,
        ( ::picolibrary::Microchip::MCP3008::Input ) );
};

} // namespace picolibrary::Testing::Unit::Microchip::MCP3008

#endif // PICOLIBRARY_TESTING_UNIT_MICROCHIP_MCP3008_H
