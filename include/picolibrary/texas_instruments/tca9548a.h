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

/**
 * \brief Caching driver.
 *
 * \tparam Bus_Multiplexer_Aligner A nullary functor. The functor must be default
 *         constructable, move constructable, and move assignable. When called, the
 *         functor should align the bus's multiplexer(s) (if any) to enable communication
 *         with the TCA9548A.
 * \tparam Controller The type of controller used to communicate with the TCA9548A.
 * \tparam Driver The type of driver to add register caching to. The default driver
 *         implementation should be used unless a mock driver implementation is being
 *         injected to support automated testing of this caching driver.
 */
template<typename Bus_Multiplexer_Aligner, typename Controller, typename Driver = ::picolibrary::Texas_Instruments::TCA9548A::Driver<Bus_Multiplexer_Aligner, Controller>>
class Caching_Driver : public Driver {
  public:
    using Driver::Driver;

    /**
     * \brief Reset all cached register values.
     */
    constexpr void reset_cache() noexcept
    {
        m_control = Control::RESET;
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
        Driver::write_control( data );

        m_control = data;
    }

    /**
     * \brief Get the cached Control register value.
     *
     * \return The cached Control register value.
     */
    constexpr auto control() const noexcept -> std::uint8_t
    {
        return m_control;
    }

  private:
    /**
     * \brief The cached Control register value.
     */
    std::uint8_t m_control{ Control::RESET };
};

/**
 * \brief Bus multiplexer aligner.
 *
 * \tparam Caching_Driver The type of caching driver used to interact with a TCA9548A.
 */
template<typename Caching_Driver>
class Bus_Multiplexer_Aligner {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Bus_Multiplexer_Aligner() noexcept = default;

    /**
     * \brief Caching driver.
     *
     * \param[in] caching_driver The caching driver used to interact with the TCA9548A.
     * \param[in] mask_enable The mask identifying the TCA9548A channels to enable.
     * \param[in] mask_disable The mask identifying the TCA9548A channels to disable.
     */
    constexpr Bus_Multiplexer_Aligner( Caching_Driver & caching_driver, std::uint8_t mask_enable, std::uint8_t mask_disable ) noexcept
        :
        m_caching_driver{ &caching_driver },
        m_mask_enable{ mask_enable },
        m_mask_disable{ mask_disable }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Bus_Multiplexer_Aligner( Bus_Multiplexer_Aligner && source ) noexcept :
        m_caching_driver{ source.m_caching_driver },
        m_mask_enable{ source.m_mask_enable },
        m_mask_disable{ source.m_mask_disable }
    {
        source.m_caching_driver = nullptr;
        source.m_mask_enable    = 0x00;
        source.m_mask_disable   = 0x00;
    }

    Bus_Multiplexer_Aligner( Bus_Multiplexer_Aligner const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Bus_Multiplexer_Aligner() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Bus_Multiplexer_Aligner && expression ) noexcept -> Bus_Multiplexer_Aligner &
    {
        if ( &expression != this ) {
            m_caching_driver = expression.m_caching_driver;
            m_mask_enable    = expression.m_mask_enable;
            m_mask_disable   = expression.m_mask_disable;

            expression.m_caching_driver = nullptr;
            expression.m_mask_enable    = 0x00;
            expression.m_mask_disable   = 0x00;
        } // if

        return *this;
    }

    auto operator=( Bus_Multiplexer_Aligner const & ) = delete;

    /**
     * \brief Configure the TCA9548A to enable communication with the device.
     */
    constexpr void operator()() const noexcept
    {
        auto const control = m_caching_driver->control();

        auto const channels_not_enabled  = ( control & m_mask_enable ) != m_mask_enable;
        auto const channels_not_disabled = ( control & m_mask_disable ) != 0x00;

        if ( channels_not_enabled or channels_not_disabled ) {
            m_caching_driver->write_control( ( control | m_mask_enable ) & ~m_mask_disable );
        } // if
    }

  private:
    /**
     * \brief The caching driver used to interact with the TCA9548A.
     */
    Caching_Driver * m_caching_driver{};

    /**
     * \brief The mask identifying the TCA9548A channels to enable.
     */
    std::uint8_t m_mask_enable{};

    /**
     * \brief The mask identifying the TCA9548A channels to disable.
     */
    std::uint8_t m_mask_disable{};
};

} // namespace picolibrary::Texas_Instruments::TCA9548A

#endif // PICOLIBRARY_TEXAS_INSTRUMENTS_TCA9548A_H
