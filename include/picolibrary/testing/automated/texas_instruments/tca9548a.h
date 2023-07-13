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
 * \brief picolibrary::Testing::Automated::Texas_Instruments::TCA9548A interface.
 */

#ifndef PICOLIBRARY_TESTING_AUTOMATED_TEXAS_INSTRUMENTS_TCA9548A_H
#define PICOLIBRARY_TESTING_AUTOMATED_TEXAS_INSTRUMENTS_TCA9548A_H

#include <cstdint>
#include <functional>

#include "gmock/gmock.h"
#include "picolibrary/error.h"
#include "picolibrary/testing/automated/i2c.h"
#include "picolibrary/texas_instruments/tca9548a.h"

/**
 * \brief Texas Instruments TCA9548A automated testing facilities.
 */
namespace picolibrary::Testing::Automated::Texas_Instruments::TCA9548A {

/**
 * \brief Mock driver.
 */
class Mock_Driver : public I2C::Mock_Device {
  public:
    Mock_Driver() = default;

    Mock_Driver(
        std::function<void()>,
        I2C::Mock_Controller &,
        ::picolibrary::Texas_Instruments::TCA9548A::Address_Transmitted,
        Error_Code const & )
    {
    }

    Mock_Driver( Mock_Driver && ) = delete;

    Mock_Driver( Mock_Driver const & ) = delete;

    ~Mock_Driver() noexcept = default;

    auto operator=( Mock_Driver && ) = delete;

    auto operator=( Mock_Driver const & ) = delete;

    MOCK_METHOD( std::uint8_t, read_control, (), ( const ) );
    MOCK_METHOD( void, write_control, ( std::uint8_t ) );
};

} // namespace picolibrary::Testing::Automated::Texas_Instruments::TCA9548A

#endif // PICOLIBRARY_TESTING_AUTOMATED_TEXAS_INSTRUMENTS_TCA9548A_H
