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
#include <utility>

#include "picolibrary/error.h"
#include "picolibrary/i2c.h"

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

/**
 * \brief Address, numeric format.
 */
using Address_Numeric = I2C::Device_Address_Numeric<0b1110'000, 0b1110'111>;

/**
 * \brief Address, transmitted format.
 */
using Address_Transmitted = I2C::Device_Address_Transmitted<0b1110'000'0, 0b1110'111'0>;

/**
 * \brief Driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor. The functor must be default
 *         constructable, move constructable, and move assignable. When called, the
 *         functor should align the bus's multiplexer(s) (if any) to enable communication
 *         with the TCA9548A.
 * \tparam Controller The type of controller used to communicate with the TCA9548A.
 * \tparam Device The type of device implementation used by the driver. The default device
 *         implementation should be used unless a mock device implementation is being
 *         injected to support automated testing of this driver.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller, typename Device = I2C::Device<Bus_Multiplexer_Aligner, Controller>>
class Driver : public Device {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] bus_multiplexer_aligner The TCA9548A's bus multiplexer aligner.
     * \param[in] controller The controller used to communicate with the TCA9548A.
     * \param[in] address The TCA9548A's address.
     * \param[in] nonresponsive_device_error The fatal error that occurs if the TCA9548A
     *            does not respond when addressed or does not acknowledge a write.
     */
    constexpr Driver(
        Bus_Multiplexer_Aligner bus_multiplexer_aligner,
        Controller &            controller,
        Address_Transmitted     address,
        Error_Code const &      nonresponsive_device_error ) noexcept :
        Device{ std::move( bus_multiplexer_aligner ), controller, std::move( address ), nonresponsive_device_error }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Driver( Driver && source ) noexcept = default;

    Driver( Driver const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Driver() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Driver && expression ) noexcept -> Driver & = default;

    auto operator=( Driver const & ) = delete;

    /**
     * \brief Read the Control register.
     *
     * \pre the TCA9548A is responsive
     *
     * \return The data read from the Control register.
     */
    auto read_control() const noexcept -> std::uint8_t
    {
        return this->read();
    }

    /**
     * \brief Write to the Control register.
     *
     * \pre the TCA9548A is responsive
     *
     * \param[in] data The data to write to the Control register.
     */
    void write_control( std::uint8_t data ) noexcept
    {
        this->write( data );
    }
};

} // namespace picolibrary::Texas_Instruments::TCA9548A

#endif // PICOLIBRARY_TEXAS_INSTRUMENTS_TCA9548A_H
