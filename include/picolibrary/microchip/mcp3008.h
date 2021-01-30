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
 * \brief picolibrary::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP3008_H
#define PICOLIBRARY_MICROCHIP_MCP3008_H

#include <cstdint>
#include <limits>

#include "picolibrary/adc.h"
#include "picolibrary/error.h"
#include "picolibrary/fixed_size_array.h"
#include "picolibrary/result.h"
#include "picolibrary/spi.h"

/**
 * \brief Microchip MCP3008 facilities.
 */
namespace picolibrary::Microchip::MCP3008 {

/**
 * \brief Microchip MCP3008 single-ended input channel.
 */
enum class Channel : std::uint8_t {
    _0 = 0b1'000'0000, ///< Channel 0.
    _1 = 0b1'001'0000, ///< Channel 1.
    _2 = 0b1'010'0000, ///< Channel 2.
    _3 = 0b1'011'0000, ///< Channel 3.
    _4 = 0b1'100'0000, ///< Channel 4.
    _5 = 0b1'101'0000, ///< Channel 5.
    _6 = 0b1'110'0000, ///< Channel 6.
    _7 = 0b1'111'0000, ///< Channel 7.
};

/**
 * \brief Microchip MCP3008 differential input channel pair.
 */
enum class Channel_Pair : std::uint8_t {
    _0_1 = 0b0'000'0000, ///< Channel 0 relative to channel 1.
    _1_0 = 0b0'001'0000, ///< Channel 1 relative to channel 0.
    _2_3 = 0b0'010'0000, ///< Channel 2 relative to channel 3.
    _3_2 = 0b0'011'0000, ///< Channel 3 relative to channel 2.
    _4_5 = 0b0'100'0000, ///< Channel 4 relative to channel 5.
    _5_4 = 0b0'101'0000, ///< Channel 5 relative to channel 4.
    _6_7 = 0b0'110'0000, ///< Channel 6 relative to channel 7.
    _7_6 = 0b0'111'0000, ///< Channel 7 relative to channel 6.
};

/**
 * \brief Microchip MCP3008 input mode/channel(s).
 */
class Input {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Input() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] channel The single-ended input channel to be sampled.
     */
    constexpr Input( Channel channel ) noexcept :
        m_control_byte{ static_cast<std::uint8_t>( channel ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] channel_pair The differential input channel pair to be sampled.
     */
    constexpr Input( Channel_Pair channel_pair ) noexcept :
        m_control_byte{ static_cast<std::uint8_t>( channel_pair ) }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Input( Input && source ) noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] original The original to copy.
     */
    constexpr Input( Input const & original ) noexcept = default;

    /**
     * \brief Destructor.
     */
    ~Input() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Input && expression ) noexcept -> Input & = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Input const & expression ) noexcept -> Input & = default;

    /**
     * \brief Get the control byte for the input mode/channel(s).
     *
     * \return The control byte for the input mode/channel(s)
     */
    constexpr explicit operator std::uint8_t() const noexcept
    {
        return m_control_byte;
    }

  private:
    /**
     * \brief The control byte for the input mode/channel(s).
     */
    std::uint8_t m_control_byte{};
};

/**
 * \brief Microchip MCP3008 sample.
 */
using Sample = ADC::Sample<std::uint_fast16_t, 0, 1023>;

/**
 * \brief Microchip MCP
 *
 * \tparam Controller_Type The type of SPI controller used to communicate with the
 *         MCP3008.
 * \tparam Device_Selector_Type The type of SPI device selector used to select and
 *         deselect the MCP3008.
 * \tparam Device The type of SPI device implementation used by the driver. The default
 *         SPI device implementation should be used unless a mock SPI device
 *         implementation is being injected to support unit testing of this driver.
 */
template<typename Controller_Type, typename Device_Selector_Type, typename Device = SPI::Device<Controller_Type, Device_Selector_Type>>
class Driver : public Device {
  public:
    /**
     * \brief The type of SPI controller used to communicate with the MCP3008.
     */
    using Controller = Controller_Type;

    /**
     * \brief The type of SPI device selector used to select and deselect the MCP3008.
     */
    using Device_Selector = Device_Selector_Type;

    /**
     * \brief Constructor.
     */
    constexpr Driver() = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The SPI controller used to communicate with the MCP3008.
     * \param[in] configuration The SPI controller clock configuration that meets the
     *            MCP3008's communication requirements.
     * \param[in] device_selector The SPI device selector used to select and deselect the
     *            MCP3008.
     * \param[in] nonresponsive The error code to return when getting a sample fails due
     *            to the MCP3008 being nonresponsive.
     */
    constexpr Driver(
        Controller &                       controller,
        typename Controller::Configuration configuration,
        Device_Selector                    device_selector,
        Error_Code                         nonresponsive ) noexcept :
        Device{ controller, configuration, std::move( device_selector ) },
        m_nonresponsive{ nonresponsive }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Driver( Driver && source ) noexcept = default;

    /**
     * \todo #29
     */
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
    auto operator=( Driver && expression ) noexcept -> Driver & = default;

    /**
     * \todo #29
     *
     * \return
     */
    auto operator=( Driver const & ) = delete;

    using Device::initialize;

    /**
     * \brief Get a sample.
     *
     * \param[in] input The input to get the sample from.
     *
     * \return A sample if getting the sample succeeded.
     * \return An error code if getting the sample failed.
     */
    auto sample( Input input ) noexcept -> Result<Sample, Error_Code>
    {
        // #lizard forgives the length

        {
            auto result = this->configure();
            if ( result.is_error() ) { return result.error(); } // if
        }

        auto data = Fixed_Size_Array<std::uint8_t, 3>{
            0x01,
            static_cast<std::uint8_t>( input ),
            0x00,
        };

        {
            auto guard = SPI::Device_Selection_Guard<Device_Selector>{};
            {
                auto result = SPI::make_device_selection_guard( this->device_selector() );
                if ( result.is_error() ) { return result.error(); } // if

                guard = std::move( result ).value();
            }

            {
                auto result = this->exchange( data.begin(), data.end(), data.begin(), data.end() );
                if ( result.is_error() ) { return result.error(); } // if
            }
        }

        if ( data[ 1 ] & 0b100 ) { return m_nonresponsive; } // if

        return Sample{ ( static_cast<Sample::Value>( data[ 1 ] & 0b11 )
                         << std::numeric_limits<std::uint8_t>::digits )
                       | data[ 2 ] };
    }

  private:
    /**
     * \brief The error code to return when getting a sample fails due to the MCP3008
     *        being nonresponsive.
     */
    Error_Code m_nonresponsive{};
};

} // namespace picolibrary::Microchip::MCP3008

#endif // PICOLIBRARY_MICROCHIP_MCP3008_H
