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
 * \brief picolibrary::Microchip::MCP3008 interface.
 */

#ifndef PICOLIBRARY_MICROCHIP_MCP3008_H
#define PICOLIBRARY_MICROCHIP_MCP3008_H

#include <cstdint>
#include <limits>
#include <utility>

#include "picolibrary/adc.h"
#include "picolibrary/array.h"
#include "picolibrary/precondition.h"
#include "picolibrary/spi.h"
#include "picolibrary/utility.h"

/**
 * \brief Microchip MCP3008 facilities.
 */
namespace picolibrary::Microchip::MCP3008 {

/**
 * \brief Input.
 */
enum class Input : std::uint8_t {
    CH0 = 0b1'000'0000, ///< Single-ended, CH0.
    CH1 = 0b1'001'0000, ///< Single-ended, CH1.
    CH2 = 0b1'010'0000, ///< Single-ended, CH2.
    CH3 = 0b1'011'0000, ///< Single-ended, CH3.
    CH4 = 0b1'100'0000, ///< Single-ended, CH4.
    CH5 = 0b1'101'0000, ///< Single-ended, CH5.
    CH6 = 0b1'110'0000, ///< Single-ended, CH6.
    CH7 = 0b1'111'0000, ///< Single-ended, CH7.

    CH0_RELATIVE_TO_CH1 = 0b0'000'0000, ///< Differential, CH0 relative to CH1.
    CH1_RELATIVE_TO_CH0 = 0b0'001'0000, ///< Differential, CH1 relative to CH0.
    CH2_RELATIVE_TO_CH3 = 0b0'010'0000, ///< Differential, CH2 relative to CH3.
    CH3_RELATIVE_TO_CH2 = 0b0'011'0000, ///< Differential, CH3 relative to CH2.
    CH4_RELATIVE_TO_CH5 = 0b0'100'0000, ///< Differential, CH4 relative to CH5.
    CH5_RELATIVE_TO_CH4 = 0b0'101'0000, ///< Differential, CH5 relative to CH4.
    CH6_RELATIVE_TO_CH7 = 0b0'110'0000, ///< Differential, CH6 relative to CH7.
    CH7_RELATIVE_TO_CH6 = 0b0'111'0000, ///< Differential, CH7 relative to CH6.
};

/**
 * \brief Sample.
 */
using Sample = ADC::Sample<std::uint_fast16_t, 10>;

/**
 * \brief Driver.
 *
 * \tparam Controller The type of controller used to communicate with the MCP3008.
 * \tparam Device_Selector The type of device selector used to select and deselect the
 *         MCP3008.
 * \tparam Device The type of device implementation used by the driver. The default device
 *         implementation should be used unless a mock device implementation is being
 *         injected to support automated testing of this driver.
 */
template<typename Controller, typename Device_Selector, typename Device = SPI::Device<Controller, Device_Selector>>
class Driver : public Device {
  public:
    /**
     * \brief Constructor.
     */
    constexpr Driver() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] controller The controller used to communicate with the MCP3008.
     * \param[in] configuration The controller clock and data exchange bit order
     *            configuration that meets the MCP3008's communication requirements.
     * \param[in] device_selector The device selector used to select and deselect the
     *            MCP3008.
     */
    constexpr Driver( Controller & controller, typename Controller::Configuration const & configuration, Device_Selector device_selector ) noexcept
        :
        Device{ controller, configuration, std::move( device_selector ) }
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

    using Device::initialize;

    /**
     * \brief Get a sample.
     *
     * \param[in] input The input to get the sample from.
     *
     * \return The sample.
     */
    auto sample( Input input ) noexcept -> Sample
    {
        auto data = Array<std::uint8_t, 3>{
            0x01,
            to_underlying( input ),
            0x00,
        };

        this->configure();

        auto const guard = SPI::Device_Selection_Guard{ this->device_selector() };

        this->exchange( data.begin(), data.end(), data.begin(), data.end() );

        return Sample{ BYPASS_PRECONDITION_EXPECTATION_CHECKS,
                       ( static_cast<Sample::Unsigned_Integer>( data[ 1 ] & 0b11 )
                         << std::numeric_limits<std::uint8_t>::digits )
                           | data[ 2 ] };
    }
};

/**
 * \brief Blocking, single sample ADC.
 *
 * \tparam Driver The type of driver used to interact with the MCP3008.
 */
template<typename Driver>
class Blocking_Single_Sample_Converter {
  public:
    /**
     * \brief Sample.
     */
    using Sample = MCP3008::Sample;

    /**
     * \brief Constructor.
     */
    constexpr Blocking_Single_Sample_Converter() noexcept = default;

    /**
     * \brief Constructor.
     *
     * \param[in] driver The driver used to interact with the MCP3008.
     * \param[in] input The input to sample.
     */
    constexpr Blocking_Single_Sample_Converter( Driver & driver, Input input ) noexcept :
        m_driver{ &driver },
        m_input{ input }
    {
    }

    /**
     * \brief Constructor.
     *
     * \param[in] source The source of the move.
     */
    constexpr Blocking_Single_Sample_Converter( Blocking_Single_Sample_Converter && source ) noexcept :
        m_driver{ source.m_driver },
        m_input{ source.m_input }
    {
        source.m_driver = nullptr;
    }

    Blocking_Single_Sample_Converter( Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Destructor.
     */
    ~Blocking_Single_Sample_Converter() noexcept = default;

    /**
     * \brief Assignment operator.
     *
     * \param[in] expression The expression to be assigned.
     *
     * \return The assigned to object.
     */
    constexpr auto operator=( Blocking_Single_Sample_Converter && expression ) noexcept
        -> Blocking_Single_Sample_Converter &
    {
        if ( &expression != this ) {
            m_driver = expression.m_driver;
            m_input  = expression.m_input;

            expression.m_driver = nullptr;
        } // if

        return *this;
    }

    auto operator=( Blocking_Single_Sample_Converter const & ) = delete;

    /**
     * \brief Initialize the ADC's hardware.
     */
    void initialize() noexcept
    {
    }

    /**
     * \brief Get a sample.
     *
     * \return The sample.
     */
    auto sample() noexcept -> Sample
    {
        return m_driver->sample( m_input );
    }

  private:
    /**
     * \brief The driver used to interact with the MCP3008.
     */
    Driver * m_driver{ nullptr };

    /**
     * \brief The input to sample.
     */
    Input m_input{};
};

} // namespace picolibrary::Microchip::MCP3008

#endif // PICOLIBRARY_MICROCHIP_MCP3008_H
